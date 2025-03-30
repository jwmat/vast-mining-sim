#include "station_manager.h"

#include <gtest/gtest.h>

#include "event.h"

TEST(TestStationManager, OneStation) {
  StationManager manager(1);

  // Check station is available
  auto [available1, id1] = manager.NextAvailableStation();
  EXPECT_EQ(available1, 0min);

  // Unload a truck at the station
  const auto arrival_time = 10min;
  auto finished1 = manager.UnloadTruck(0, arrival_time);
  EXPECT_EQ(finished1, arrival_time + StationManager::kUnloadTime);

  auto& events = GetEventLogger().GetEvents();
  ASSERT_FALSE(events.empty());

  // Check the event is logged correctly
  const auto& event = events.back();
  EXPECT_EQ(event.type, EventType::Unload);
  EXPECT_EQ(event.truck_id, 0);
  EXPECT_EQ(event.station_id, id1);
  EXPECT_EQ(event.start_time, arrival_time);
  EXPECT_EQ(event.end_time, arrival_time + StationManager::kUnloadTime);

  // Check the next station is properly in the queue
  auto [available2, id2] = manager.NextAvailableStation();
  EXPECT_EQ(id1, id2);
  EXPECT_EQ(available2, finished1);
}

TEST(TestStationManager, TwoStations) {
  StationManager manager(2);
  const minutes_t start = 0min;
  const minutes_t arrival_time1 = 100min;

  // Unload truck at station1
  auto [available1, id1] = manager.NextAvailableStation();
  EXPECT_EQ(available1, start);
  auto finished1 = manager.UnloadTruck(0, arrival_time1);
  EXPECT_EQ(finished1, arrival_time1 + StationManager::kUnloadTime);

  auto& events = GetEventLogger().GetEvents();
  ASSERT_FALSE(events.empty());

  // Check the event is logged correctly
  const auto& event1 = events.back();
  EXPECT_EQ(event1.type, EventType::Unload);
  EXPECT_EQ(event1.truck_id, 0);
  EXPECT_EQ(event1.station_id, id1);
  EXPECT_EQ(event1.start_time, arrival_time1);
  EXPECT_EQ(event1.end_time, arrival_time1 + StationManager::kUnloadTime);

  // Next available station should be station2
  auto [available2, id2] = manager.NextAvailableStation();
  EXPECT_NE(id1, id2);
  EXPECT_EQ(available2, start);

  // Unload truck with a shorter arrival time
  const auto arrival_time2 = arrival_time1 / 2;
  const auto finished2 = manager.UnloadTruck(1, arrival_time2);
  EXPECT_EQ(finished2, arrival_time2 + StationManager::kUnloadTime);

  // Check the event is logged correctly
  const auto& event2 = events.back();
  EXPECT_EQ(event2.type, EventType::Unload);
  EXPECT_EQ(event2.truck_id, 1);
  EXPECT_EQ(event2.station_id, id2);
  EXPECT_EQ(event2.start_time, arrival_time2);
  EXPECT_EQ(event2.end_time, arrival_time2 + StationManager::kUnloadTime);

  // Next station should still be the station2 who finished sooner
  auto [time3, id3] = manager.NextAvailableStation();
  EXPECT_EQ(id3, id2);
  EXPECT_EQ(time3, finished2);

  // Unload truck that should queue
  const auto finished3 = manager.UnloadTruck(2, arrival_time2);
  EXPECT_EQ(finished3, time3 + StationManager::kUnloadTime);

  // Check the event is logged correctly
  const auto& event3 = events.at(events.size() - 2);
  EXPECT_EQ(event3.type, EventType::Queue);
  EXPECT_EQ(event3.truck_id, 2);
  EXPECT_EQ(event3.station_id, id2);
  EXPECT_EQ(event3.start_time, arrival_time2);
  EXPECT_EQ(event3.end_time, time3);

  const auto& event4 = events.back();
  EXPECT_EQ(event4.type, EventType::Unload);
  EXPECT_EQ(event4.truck_id, 2);
  EXPECT_EQ(event4.station_id, id2);
  EXPECT_EQ(event4.start_time, time3);
  EXPECT_EQ(event4.end_time, time3 + StationManager::kUnloadTime);
}

TEST(TestStationManager, NoStations) {
  StationManager manager(0);
  ASSERT_ANY_THROW(manager.NextAvailableStation());
}
