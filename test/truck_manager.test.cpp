#include <gtest/gtest.h>

#include "truck_manager.h"

TEST(TestTruckManager, DispatchOneTruck) {
  TruckManager manager(1);

  // Check a truck is available
  ASSERT_EQ(manager.TrucksAvailable(), 1);
  auto [time1, id1] = manager.NextAvailableTruck();
  EXPECT_EQ(time1, 0min);

  // Check truck "mines" for duration
  const minutes_t duration = 100min;
  manager.DispatchToMine(id1, time1, duration);
  auto [time2, id2] = manager.NextAvailableTruck();
  EXPECT_EQ(id2, id1);
  EXPECT_EQ(time2, time1 + duration + TruckManager::kTravelTime);
}

TEST(TestTruckManager, DispatchTwoTrucks) {
  TruckManager manager(2);
  const minutes_t start = 0min;
  const minutes_t duration1 = 100min;
  ASSERT_EQ(manager.TrucksAvailable(), 2);

  // Dispatch truck1 to mine
  auto [time1, id1] = manager.NextAvailableTruck();
  EXPECT_EQ(time1, start);
  manager.DispatchToMine(id1, time1, duration1);
  // Next available truck should be truck2 idle at the truck depot
  auto [time2, id2] = manager.NextAvailableTruck();
  EXPECT_NE(id1, id2);
  EXPECT_EQ(time2, start);
  // Dispatch truck2 to the mines with a duration shorter than truck1
  const auto duration2 = duration1 / 2;
  manager.DispatchToMine(id2, time2, duration2);
  // Next truck should still be the truck2 who finished sooner
  auto [time3, id3] = manager.NextAvailableTruck();
  EXPECT_EQ(id3, id2);
  EXPECT_EQ(time3, time2 + duration2 + TruckManager::kTravelTime);
  // Redispatch truck2 with a long time;
  const auto duration3 = duration1 * 2;
  manager.DispatchToMine(id3, time3, duration3);
  // Next truck available should be the truck1
  auto [time4, id4] = manager.NextAvailableTruck();
  EXPECT_EQ(id4, id1);
  EXPECT_EQ(time4, time1 + duration1 + TruckManager::kTravelTime);
}

TEST(TestTruckManager, NoTrucks) {
  TruckManager manager(0);
  EXPECT_EQ(manager.TrucksAvailable(), 0);
  ASSERT_ANY_THROW(manager.NextAvailableTruck());
}
