#include <gtest/gtest.h>

#include "controller.h"
#include "report.h"

TEST(TestMetrics, TruckWithNoTrips) {
  TruckMetrics t{};
  std::vector<TruckMetrics> trucks{t};
  std::vector<StationMetrics> stations;

  GenerateMetrics(60min, &trucks, &stations);

  EXPECT_DOUBLE_EQ(trucks[0].utilization, 0.0);
  EXPECT_DOUBLE_EQ(trucks[0].avg_trip_time, 0.0);
}

TEST(TestMetrics, SingleTripTruckMetrics) {
  TruckMetrics t{};
  t.trips_completed = 1;
  t.mining_time = 10min;
  t.mines_completed = 1;
  t.travel_time = 20min;
  t.unloading_time = 30min;

  std::vector<TruckMetrics> trucks{t};
  std::vector<StationMetrics> stations;

  GenerateMetrics(60min, &trucks, &stations);

  EXPECT_NEAR(trucks[0].utilization, 100.0, 1e-2);
  EXPECT_DOUBLE_EQ(trucks[0].avg_trip_time, 60.0);
}

TEST(TestMetrics, StationUtilizationMatchesThroughput) {
  StationMetrics s{};
  s.throughput = 2;
  s.unloading_time = s.throughput * Controller::kUnloadTime;

  std::vector<TruckMetrics> trucks;
  std::vector<StationMetrics> stations{s};

  GenerateMetrics(120min, &trucks, &stations);
  EXPECT_GT(stations[0].utilization, 0.0);
}
