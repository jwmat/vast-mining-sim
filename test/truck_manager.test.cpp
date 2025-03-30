#include "truck_manager.h"

#include <gtest/gtest.h>

#include "event.h"

TEST(TestTruckManager, DispatchOneTruck) {
  TruckManager manager(1);

  // Check a truck is available
  ASSERT_EQ(manager.TrucksAvailable(), 1);
  auto [time1, id1] = manager.NextAvailableTruck();
  EXPECT_EQ(time1, 0min);

  // Check truck "mines" for duration
  const minutes_t duration = 100min;
  manager.DispatchTruckToMine(id1, time1, duration);

  auto& events = GetEventLogger().GetEvents();
  ASSERT_FALSE(events.empty());

  // Check the event is logged correctly
  const auto& event = events.back();
  EXPECT_EQ(event.type, EventType::Mine);
  EXPECT_EQ(event.truck_id, id1);
  EXPECT_EQ(event.start_time, time1);
  EXPECT_EQ(event.end_time, time1 + duration);

  // Check out queue was populated correctly
  auto [time2, id2] = manager.NextAvailableTruck();
  EXPECT_EQ(id2, id1);
  EXPECT_EQ(time2, time1 + duration);
}

TEST(TestTruckManager, DispatchTwoTrucks) {
  TruckManager manager(2);
  const minutes_t start = 0min;
  const minutes_t duration1 = 100min;
  ASSERT_EQ(manager.TrucksAvailable(), 2);

  // Dispatch truck1 to mine
  auto [time1, id1] = manager.NextAvailableTruck();
  EXPECT_EQ(time1, start);
  manager.DispatchTruckToMine(id1, time1, duration1);

  auto& events = GetEventLogger().GetEvents();
  ASSERT_FALSE(events.empty());

  // Check the event is logged correctly
  const auto& event1 = events.back();
  EXPECT_EQ(event1.type, EventType::Mine);
  EXPECT_EQ(event1.truck_id, id1);
  EXPECT_EQ(event1.start_time, time1);
  EXPECT_EQ(event1.end_time, time1 + duration1);

  // Next available truck should be truck2 idle at the truck depot
  auto [time2, id2] = manager.NextAvailableTruck();
  EXPECT_NE(id1, id2);
  EXPECT_EQ(time2, start);

  // Dispatch truck2 to the mines with a duration shorter than truck1
  const auto duration2 = duration1 / 2;
  manager.DispatchTruckToMine(id2, time2, duration2);

  // Check the event is logged correctly
  const auto& event2 = events.back();
  EXPECT_EQ(event2.type, EventType::Mine);
  EXPECT_EQ(event2.truck_id, id2);
  EXPECT_EQ(event2.start_time, time2);
  EXPECT_EQ(event2.end_time, time2 + duration2);

  // Next truck should still be the truck2 who finished sooner
  auto [time3, id3] = manager.NextAvailableTruck();
  EXPECT_EQ(id3, id2);
  EXPECT_EQ(time3, time2 + duration2);

  // Redispatch truck2 with a long time;
  const auto duration3 = duration1 * 2;
  manager.DispatchTruckToMine(id3, time3, duration3);

  const auto& event3 = events.back();
  EXPECT_EQ(event3.type, EventType::Mine);
  EXPECT_EQ(event3.truck_id, id3);
  EXPECT_EQ(event3.start_time, time3);
  EXPECT_EQ(event3.end_time, time3 + duration3);

  // Next truck available should be the truck1
  auto [time4, id4] = manager.NextAvailableTruck();
  EXPECT_EQ(id4, id1);
  EXPECT_EQ(time4, time1 + duration1);
}

TEST(TestTruckManager, NoTrucks) {
  TruckManager manager(0);
  EXPECT_EQ(manager.TrucksAvailable(), 0);
  ASSERT_ANY_THROW(manager.NextAvailableTruck());
}

TEST(TestTruckManager, NextAvailable) {
  TruckManager truck_manager(2);

  ASSERT_EQ(truck_manager.TrucksAvailable(), 2);

  // First available truck (dequeues truck)
  auto [truck1_start_time, truck1_id] = truck_manager.NextAvailableTruck();
  EXPECT_EQ(truck1_start_time, 0min);

  // Send truck1 to mine (enqueues truck)
  const auto truck1_mining_time = 31min;
  const auto truck1_end_time = truck_manager.DispatchTruckToMine(
      truck1_id, truck1_start_time, truck1_mining_time);
  EXPECT_EQ(truck1_end_time, truck1_start_time + truck1_mining_time);

  // Send truck2 to mine with shorter mining duration
  auto [truck2_start_time, truck2_id] = truck_manager.NextAvailableTruck();
  EXPECT_EQ(truck2_start_time, 0min);

  const auto truck2_mining_time = 30min;
  const auto truck2_end_time = truck_manager.DispatchTruckToMine(
      truck2_id, truck2_start_time, truck2_mining_time);
  EXPECT_EQ(truck1_end_time, truck2_start_time + truck1_mining_time);

  // Truck 2 should finish first
  auto [truck3_start_time, truck3_id] = truck_manager.NextAvailableTruck();
  EXPECT_EQ(truck3_id, truck2_id);
  EXPECT_EQ(truck3_start_time, truck2_end_time);
}
