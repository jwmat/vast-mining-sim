#include <gtest/gtest.h>

#include "station_manager.h"

TEST(TestStationManager, OneStation) {
  StationManager manager(1);

  // Check station is available
  auto [available1, id1] = manager.NextAvailableStation();
  EXPECT_EQ(available1, 0min);

  // Unload a truck at the station
  const auto arrival_time = 10min;
  auto finished1 = manager.UnloadTruck(id1, 0min, arrival_time);
  EXPECT_EQ(finished1, arrival_time + StationManager::kUnloadTime);

  // Check the next station is properly in the queue
  auto [available2, id2] = manager.NextAvailableStation();
  EXPECT_EQ(id1, id2);
  EXPECT_EQ(available2, finished1);

  // Check that a station properly queues
  const minutes_t available = 500min;
  auto finished2 = manager.UnloadTruck(id1, available, 0min);
  EXPECT_EQ(finished2, available + StationManager::kUnloadTime);
}

TEST(TestStationManager, TwoStations) {
  StationManager manager(2);
  const minutes_t start = 0min;
  const minutes_t arrival_time1 = 100min;

  // Unload truck at station1
  auto [available1, id1] = manager.NextAvailableStation();
  EXPECT_EQ(available1, start);
  auto finished1 = manager.UnloadTruck(id1, 0min, arrival_time1);
  EXPECT_EQ(finished1, arrival_time1 + StationManager::kUnloadTime);

  // Next available station should be station2
  auto [available2, id2] = manager.NextAvailableStation();
  EXPECT_NE(id1, id2);
  EXPECT_EQ(available2, start);

  // Unload truck with a shorter arrival time
  const auto arrival_time2 = arrival_time1 / 2;
  const auto finished2 = manager.UnloadTruck(id2, 0min, arrival_time2);
  EXPECT_EQ(finished2, arrival_time2 + StationManager::kUnloadTime);

  // Next station should still be the station2 who finished sooner
  auto [time3, id3] = manager.NextAvailableStation();
  EXPECT_EQ(id3, id2);
  EXPECT_EQ(time3, finished2);

  // Unload truck with a long wait time
  const auto available = 500min;
  const auto finished3 = manager.UnloadTruck(id3, available, 0min);
  EXPECT_EQ(finished3, available + StationManager::kUnloadTime);

  // Next station available should be the station1
  auto [time4, id4] = manager.NextAvailableStation();
  EXPECT_EQ(id4, id1);
  EXPECT_EQ(time4, finished1);
}

TEST(TestStationManager, NoStations) {
  StationManager manager(0);
  ASSERT_ANY_THROW(manager.NextAvailableStation());
}
