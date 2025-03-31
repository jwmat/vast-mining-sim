#include "station_manager.h"

#include <gtest/gtest.h>

#include "event.h"

// Verifies that unloading a truck logs the correct event and the station is
// requeued
TEST(TestStationManager, SingleStationUnloadAndRequeue) {
  StationManager manager(1);
  auto& events = GetEventLogger().GetEvents();
  events.clear();
  size_t truck_id = 0;

  // Unload a truck at the station
  const auto start_time1 = 10min;
  auto [end_time1, station_id1] = manager.UnloadTruck(truck_id, start_time1);
  EXPECT_EQ(end_time1, start_time1 + StationManager::kUnloadTime);
  ASSERT_FALSE(events.empty());

  // Check that the event was logged correctly
  const auto& event = events.back();
  EXPECT_EQ(event.type, EventType::Unload);
  EXPECT_EQ(event.truck_id, truck_id);
  EXPECT_EQ(event.station_id, station_id1);
  EXPECT_EQ(event.start_time, start_time1);
  EXPECT_EQ(event.end_time, end_time1);

  // Ensure the station is next available at the correct time
  auto [start_time2, station_id2] = manager.NextAvailableStation();
  EXPECT_EQ(station_id1, station_id2);
  EXPECT_EQ(start_time2, end_time1);
}

// Verifies station selection and queuing behavior with two stations
TEST(TestStationManager, TwoStations_SelectAndQueueCorrectly) {
  StationManager manager(2);
  auto& events = GetEventLogger().GetEvents();
  events.clear();
  const size_t truck_id = 0;
  const size_t station1_id = 0, station2_id = 1;

  // First truck unloads at station 0 at t=10
  const auto start_time1 = 10min;
  auto [end_time1, station_id1] = manager.UnloadTruck(truck_id, start_time1);
  EXPECT_EQ(station_id1, station1_id);
  EXPECT_EQ(end_time1, start_time1 + StationManager::kUnloadTime);
  ASSERT_FALSE(events.empty());

  const auto& event1 = events.back();
  EXPECT_EQ(event1.type, EventType::Unload);
  EXPECT_EQ(event1.truck_id, truck_id);
  EXPECT_EQ(event1.station_id, station_id1);
  EXPECT_EQ(event1.start_time, start_time1);
  EXPECT_EQ(event1.end_time, end_time1);

  // Second truck unloads at station 1 with earlier start time
  const auto start_time2 = start_time1 / 2;
  const auto [end_time2, station_id2] =
      manager.UnloadTruck(truck_id, start_time2);
  EXPECT_EQ(end_time2, start_time2 + StationManager::kUnloadTime);

  const auto& event2 = events.back();
  EXPECT_EQ(event2.type, EventType::Unload);
  EXPECT_EQ(event2.truck_id, truck_id);
  EXPECT_EQ(event2.station_id, station2_id);
  EXPECT_EQ(event2.start_time, start_time2);
  EXPECT_EQ(event2.end_time, end_time2);

  // Next available should be station 1 (finished earlier)
  auto [start_time3, station_id3] = manager.NextAvailableStation();
  EXPECT_EQ(station_id3, station2_id);
  EXPECT_EQ(start_time3, end_time2);

  // Third truck arrives at same time again -> should queue
  const auto [end_time3, station_id4] =
      manager.UnloadTruck(truck_id, start_time2);
  EXPECT_EQ(end_time3, end_time2 + StationManager::kUnloadTime);

  // Verify the queue event was logged correctly
  const auto& event3 =
      events.at(events.size() - 2);  // Queue comes before unload
  EXPECT_EQ(event3.type, EventType::Queue);
  EXPECT_EQ(event3.truck_id, truck_id);
  EXPECT_EQ(event3.station_id, station2_id);
  EXPECT_EQ(event3.start_time, start_time2);
  EXPECT_EQ(event3.end_time, end_time2);
}

// Ensures StationManager throws when no stations are configured
TEST(TestStationManager, ThrowsIfNoStationsAvailable) {
  StationManager manager(0);
  ASSERT_THROW(manager.NextAvailableStation(), std::runtime_error);
}

// Ensures that trucks queued at the same station unload in the order they
// arrived
TEST(TestStationManager, QueuedTrucksServeFIFO) {
  StationManager manager(1);
  auto& events = GetEventLogger().GetEvents();
  events.clear();

  const size_t truck1_id = 0;
  const size_t truck2_id = 1;

  const auto start_time1 = 0min;
  const auto [end_time1, station_id1] =
      manager.UnloadTruck(truck1_id, start_time1);

  const auto start_time2 =
      StationManager::kUnloadTime - 1min;  // Arrives while truck1 is unloading
  const auto [end_time2, station_id2] =
      manager.UnloadTruck(truck2_id, start_time2);

  const auto& event1 =
      events.at(events.size() - 2);  // Queue comes before unload
  EXPECT_EQ(event1.type, EventType::Queue);
  EXPECT_EQ(event1.truck_id, truck2_id);

  const auto& event2 = events.back();
  EXPECT_EQ(event2.type, EventType::Unload);
  EXPECT_EQ(event2.truck_id, truck2_id);
  EXPECT_LT(event1.start_time, event2.start_time);
}

// Ensures no Queue event is logged if the station is free at the truck's
// arrival time
TEST(TestStationManager, NoQueueIfStationFree) {
  StationManager manager(1);
  auto& events = GetEventLogger().GetEvents();
  events.clear();

  const size_t truck_id = 0;

  const auto [end_time1, station_id1] = manager.UnloadTruck(truck_id, 0min);
  const auto [end_time2, station_id2] =
      manager.UnloadTruck(truck_id, end_time1);

  const auto& event = events.back();
  EXPECT_EQ(event.type, EventType::Unload);
  EXPECT_EQ(event.start_time, end_time1);

  for (const auto& event : events) {
    EXPECT_NE(event.type, EventType::Queue);  // Ensure no queuing occurred
  }
}
