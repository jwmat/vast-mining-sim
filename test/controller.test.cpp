#include "controller.h"

#include <gtest/gtest.h>

#include "event.h"
#include "logger.h"
#include "minutes.h"

// Struct to hold simulation parameters for parameterized tests
struct SimParams {
  size_t num_trucks;
  size_t num_stations;
  minutes_t sim_time;
};

// Enables GTest to print simulation parameters clearly
std::ostream& operator<<(std::ostream& os, const SimParams& p) {
  os << "{ trucks=" << p.num_trucks << ", stations=" << p.num_stations
     << ", sim_time=" << p.sim_time.count() << "min }";
  return os;
}

// Parameterized test fixture for varying truck/station/time scenarios
class TestController_WithParams : public ::testing::TestWithParam<SimParams> {
 protected:
  void SetUp() override { ClearEvents(); }  // Clear event log before each test
};

// Defines a variety of simulation configurations to run tests against
INSTANTIATE_TEST_SUITE_P(SimulationVariants, TestController_WithParams,
                         ::testing::Values(SimParams{30, 10, 1 * 60min},
                                           SimParams{150, 50, 30 * 60min},
                                           SimParams{300, 100, 72 * 60min},
                                           SimParams{300, 5, 1 * 60min},
                                           SimParams{600, 10, 30 * 60min},
                                           SimParams{1000, 20, 72 * 60min},
                                           SimParams{5, 50, 1 * 60min},
                                           SimParams{20, 100, 30 * 60min},
                                           SimParams{40, 200, 72 * 60min}));

// If simulation time is too short, no events should be generated
TEST(TestController, NoTimeToMine) {
  ClearEvents();
  Controller controller(10, 2);
  controller.Run(Controller::kMinDuration - 1min);
  Event event;
  EXPECT_FALSE(GetEventLogger().ReadNextEvent(&event));
}

// Confirms the exact sequence and timing of events for a single truck
TEST(TestController, SequenceOfEvents) {
  ClearEvents();
  Controller controller(1, 1);
  controller.Run(24 * 60min);

  const std::vector<EventType> expected_sequence = {
      EventType::Mine,
      EventType::TravelToStation,
      EventType::Unload,
      EventType::TravelToMine,
  };

  auto start_time = 0min;
  for (size_t i = 0; i < expected_sequence.size(); i++) {
    Event event;
    ReadEvent(&event);
    EXPECT_EQ(event.type, expected_sequence[i])
        << "Event types don't match : " << EventTypeToString(event.type) << " "
        << EventTypeToString(expected_sequence[i]);
    EXPECT_EQ(event.start_time, start_time);
    start_time = event.end_time;
  }
}

// Edge case: No trucks means no events
TEST(TestController, NoTrucksNoEvents) {
  ClearEvents();
  Controller controller(0, 1);
  controller.Run(60min);
  Event event;
  EXPECT_FALSE(ReadEvent(&event));
}

// Edge case: No stations available, trucks should mine but never unload
TEST(TestController, NoStationsHandledGracefully) {
  ClearEvents();
  Controller controller(10, 0);
  controller.Run(60min);
  bool saw_unload = false;
  Event event;
  while (ReadEvent(&event)) {
    ASSERT_NE(event.type, EventType::Unload);
    if (event.type == EventType::Unload) saw_unload = true;
  }
  EXPECT_FALSE(saw_unload);
}

// Ensures each truck mines at least once and the simulation makes progress
TEST_P(TestController_WithParams, EachTruckMinesAtLeastOnce) {
  const size_t num_trucks = 5;
  Controller controller(num_trucks, 1);
  controller.Run(48 * 60min);

  std::vector<size_t> mine_count(num_trucks, 0);
  size_t unload_count = 0;

  Event event;
  while (ReadEvent(&event)) {
    if (event.type == EventType::Mine) {
      mine_count[event.truck_id]++;
    }
    if (event.type == EventType::Unload) {
      unload_count++;
    }
  }

  EXPECT_GT(unload_count, num_trucks);
  for (const auto& count : mine_count) {
    EXPECT_GT(count, 1);
  }
}

// Min-heap to compare event order by timestamp
using MinHeap = std::priority_queue<std::pair<minutes_t, Event>,
                                    std::vector<std::pair<minutes_t, Event>>,
                                    std::greater<>>;

// Verifies that trucks unload in the same order they finished mining
TEST_P(TestController_WithParams, UnloadOrderMatchesMiningOrder) {
  Controller controller(10, 1);
  controller.Run(72 * 60min);

  MinHeap mining_order;
  MinHeap unloading_order;

  Event event;
  while (ReadEvent(&event)) {
    if (event.type == EventType::Mine)
      mining_order.push({event.end_time, event});
    if (event.type == EventType::Unload)
      unloading_order.push({event.start_time, event});
  }

  while (!mining_order.empty() && !unloading_order.empty()) {
    ASSERT_EQ(mining_order.top().second.truck_id,
              unloading_order.top().second.truck_id);
    mining_order.pop();
    unloading_order.pop();
  }
}

// Confirms no truck or station is double-booked at the same time
TEST_P(TestController_WithParams, TimesDoNotOverlap) {
  size_t num_trucks = 15;
  size_t num_stations = 4;

  Controller controller(num_trucks, num_stations);
  controller.Run(10 * 60min);

  std::vector<minutes_t> truck_times(num_trucks, 0min);
  std::vector<minutes_t> station_times(num_stations, 0min);

  Event event;
  while (ReadEvent(&event)) {
    if (event.type == EventType::Unload && event.station_id) {
      auto sid = event.station_id.value();
      ASSERT_GE(event.start_time, station_times[sid]);
      station_times[sid] = event.end_time;
    }
    if (event.type != EventType::Queue) {
      ASSERT_GE(event.start_time, truck_times[event.truck_id]);
      truck_times[event.truck_id] = event.end_time;
    }
  }
}

// Ensures no event exceeds the simulation time boundaries
TEST_P(TestController_WithParams, NoEventsExceedSimTime) {
  const auto& params = GetParam();
  Controller controller(params.num_trucks, params.num_stations);
  controller.Run(params.sim_time);

  Event event;
  while (ReadEvent(&event)) {
    EXPECT_LE(event.start_time, params.sim_time) <<
        "Start time exceeds sim time: " << event;
    EXPECT_LE(event.end_time, params.sim_time)
        << "End time exceeds sim time: " << event;
    if (event.type == EventType::Queue) {
      EXPECT_LT(event.start_time, event.end_time);
    }
  }
}

TEST(TestController, NoTruckOrStationOverlaps) {
  ClearEvents();
  Controller controller(/*num_trucks=*/25, /*num_stations=*/4);
  controller.Run(48 * 60min);

  std::unordered_map<size_t, std::vector<std::pair<minutes_t, minutes_t>>>
      truck_events;
  std::unordered_map<size_t, std::vector<std::pair<minutes_t, minutes_t>>>
      station_unloads;

  Event event;
  while (ReadEvent(&event)) {
    // Track per-truck events
    truck_events[event.truck_id].emplace_back(event.start_time, event.end_time);

    // Only track unloads per station
    if (event.type == EventType::Unload && event.station_id.has_value()) {
      station_unloads[event.station_id.value()].emplace_back(event.start_time,
                                                             event.end_time);
    }
  }

  // Check each truck's events are sequential (no overlaps)
  for (const auto& [truck_id, intervals] : truck_events) {
    auto sorted = intervals;
    std::sort(sorted.begin(), sorted.end());

    for (size_t i = 1; i < sorted.size(); ++i) {
      EXPECT_GE(sorted[i].first, sorted[i - 1].second)
          << "Truck " << truck_id << " has overlapping events: ["
          << sorted[i - 1].first.count() << ", " << sorted[i - 1].second.count()
          << "] overlaps with [" << sorted[i].first.count() << ", "
          << sorted[i].second.count() << "]";
    }
  }

  // Check each station only unloads one truck at a time
  for (const auto& [station_id, intervals] : station_unloads) {
    auto sorted = intervals;
    std::sort(sorted.begin(), sorted.end());

    for (size_t i = 1; i < sorted.size(); ++i) {
      EXPECT_GE(sorted[i].first, sorted[i - 1].second)
          << "Station " << station_id << " has overlapping unloads: ["
          << sorted[i - 1].first.count() << ", " << sorted[i - 1].second.count()
          << "] overlaps with [" << sorted[i].first.count() << ", "
          << sorted[i].second.count() << "]";
    }
  }
}
