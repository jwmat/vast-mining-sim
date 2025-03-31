#include "truck_manager.h"

#include <gtest/gtest.h>

#include "event.h"

// Verifies that a single truck can be dispatched, removed from availability,
// and returned
TEST(TestTruckManager, DispatchAndReturnSingleTruck) {
  TruckManager manager(1);

  ASSERT_EQ(manager.TrucksAvailable(), 1);

  const auto start_time = 0min;
  const size_t truck_id = 0;

  // Dispatch truck
  const auto [start_time1, truck_id1] = manager.DispatchTruck();
  EXPECT_EQ(truck_id, truck_id1);
  EXPECT_EQ(start_time1, start_time);

  // Truck queue should now be empty
  EXPECT_EQ(manager.TrucksAvailable(), 0);
  EXPECT_ANY_THROW(manager.DispatchTruck());

  // Return the truck
  const auto time = 50min;
  manager.ReturnTruck(truck_id, time);
  ASSERT_EQ(manager.TrucksAvailable(), 1);

  // Re-dispatch the same truck
  const auto [start_time2, truck_id2] = manager.DispatchTruck();
  EXPECT_EQ(truck_id, truck_id2);
  EXPECT_EQ(start_time2, time);
}

// Verifies dispatch order and requeue behavior with two trucks
TEST(TestTruckManager, DispatchAndPrioritizeTwoTrucks) {
  TruckManager manager(2);

  ASSERT_EQ(manager.TrucksAvailable(), 2);
  const auto start_time = 0min;
  const size_t truck1_id = 0, truck2_id = 1;

  // Dispatch truck 1
  const auto [start_time1, truck_id1] = manager.DispatchTruck();
  EXPECT_EQ(truck_id1, truck_id1);
  EXPECT_EQ(start_time1, start_time);

  // Return truck 1 later
  const auto time1 = 50min;
  manager.ReturnTruck(truck1_id, time1);

  // Dispatch truck 2
  const auto [start_time2, truck_id2] = manager.DispatchTruck();
  EXPECT_EQ(truck_id2, truck2_id);
  EXPECT_EQ(start_time2, start_time);
  EXPECT_EQ(manager.TrucksAvailable(), 1);

  // Return truck 2 earlier
  const auto time2 = 10min;
  manager.ReturnTruck(truck2_id, time2);
  EXPECT_EQ(manager.TrucksAvailable(), 2);

  // Next truck should be truck 2 (earlier return)
  const auto [start_time3, truck_id3] = manager.DispatchTruck();
  EXPECT_EQ(truck_id3, truck2_id);
  EXPECT_EQ(start_time3, time2);
  EXPECT_EQ(manager.TrucksAvailable(), 1);

  // Next truck should now be truck 1
  const auto [start_time4, truck_id4] = manager.DispatchTruck();
  EXPECT_EQ(truck_id4, truck1_id);
  EXPECT_EQ(start_time4, time1);
  EXPECT_EQ(manager.TrucksAvailable(), 0);
}

//  Ensures TruckManager correctly reports zero availability when no trucks are
//  initialized
TEST(TestTruckManager, NoTrucksAvailable) {
  TruckManager manager(0);
  EXPECT_EQ(manager.TrucksAvailable(), 0);
}

// Ensures that a truck becomes available only at its scheduled return
TEST(TestTruckManager, ReturnedTruckAvailableAtCorrectTime) {
  TruckManager manager(1);

  const auto [start_time1, truck_id1] = manager.DispatchTruck();
  const auto return_time = 100min;
  manager.ReturnTruck(truck_id1, return_time);

  const auto [start_time2, truck_id2] = manager.DispatchTruck();
  EXPECT_EQ(truck_id2, truck_id1);
  EXPECT_EQ(start_time2, return_time);
}

// Ensures trucks are dispatched in order based on their return times
TEST(TestTruckManager, DispatchOrderRespectsReturnTime) {
  TruckManager manager(3);

  // Dispatch all trucks
  const auto [start_time1, truck1_id] = manager.DispatchTruck();
  const auto [start_time2, truck2_id] = manager.DispatchTruck();
  const auto [start_time3, truck3_id] = manager.DispatchTruck();

  // Return trucks in a shuffled order
  manager.ReturnTruck(truck1_id, 30min);  // Middle
  manager.ReturnTruck(truck2_id, 10min);  // Earliest
  manager.ReturnTruck(truck3_id, 50min);  // Latest

  // Dispatch should follow return time order
  const auto [start_time4, truck_id4] = manager.DispatchTruck();
  EXPECT_EQ(truck_id4, truck2_id);
  EXPECT_EQ(start_time4, 10min);

  const auto [start_time5, truck_id5] = manager.DispatchTruck();
  EXPECT_EQ(truck_id5, truck1_id);
  EXPECT_EQ(start_time5, 30min);

  const auto [start_time6, truck_id6] = manager.DispatchTruck();
  EXPECT_EQ(truck_id6, truck3_id);
  EXPECT_EQ(start_time6, 50min);
}

// Ensures that returning a truck that was never dispatched throws an error
TEST(TestTruckManager, ReturnWithoutDispatchThrows) {
  TruckManager manager(1);
  // Attempting to return a truck that was not dispatched should throw
  EXPECT_THROW(manager.ReturnTruck(0, 100min), std::runtime_error);
}
