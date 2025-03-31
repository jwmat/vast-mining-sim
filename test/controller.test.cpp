#include "controller.h"

#include <gtest/gtest.h>

#include "event.h"
#include "heap.h"
#include "logger.h"
#include "minutes.h"

struct SimParams {
  size_t num_trucks;
  size_t num_stations;
  minutes_t sim_time;
};

inline std::ostream& operator<<(std::ostream& os, const SimParams& p) {
  os << "{ trucks=" << p.num_trucks << ", stations=" << p.num_stations
     << ", sim_time=" << p.sim_time.count() << "min }";
  return os;
}

class TestController_WithParams : public ::testing::TestWithParam<SimParams> {
 protected:
  void SetUp() override {
    // Clear the event log before each run
    GetEventLogger().GetEvents().clear();
    // SetLogger(std::make_shared<Logger>());
  }
};

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

// Ensures no trucks are dispatched if simulation time is too short
TEST(TestController, NoTimeToMine) {
  Controller controller(10, 2);
  controller.Run(MineManager::kMinDuration - 1min);
  EXPECT_EQ(GetEventLogger().GetEvents().size(), 0);
}

// Verifies correct sequence and timing of events in the simulation
TEST(TestController, SequenceOfEvents) {
  Controller controller(1, 1);

  controller.Run(24 * 60min);

  const std::vector<EventType> expected_sequence = {
      EventType::Mine,
      EventType::Travel,
      EventType::Unload,
      EventType::Travel,
  };

  const auto& events = GetEventLogger().GetEvents();
  ASSERT_GE(events.size(), expected_sequence.size());

  // Ensure event types and timing align exactly
  auto start_time = 0min;
  for (int i = 0; i < expected_sequence.size(); i++) {
    EXPECT_EQ(events[i].type, expected_sequence[i]);
    EXPECT_EQ(events[i].start_time, start_time);
    start_time = events[i].end_time;
  }
}

// Verifies that every truck is dispatched to mine at least once
TEST_P(TestController_WithParams, EachTruckMinesAtLeastOnce) {
  const size_t num_trucks = 5;

  Controller controller(num_trucks, /*num_stations=*/1);
  controller.Run(48 * 60min);  // Simulate 48 hours

  std::vector<size_t> mine_count(num_trucks, 0);
  size_t unload_count = 0;

  const auto& events = GetEventLogger().GetEvents();
  ASSERT_GT(events.size(), 0);  // Sanity check: simulation produced events

  for (const auto& event : events) {
    if (event.type == EventType::Mine) {
      mine_count[event.truck_id]++;
    }
    if (event.type == EventType::Unload) {
      unload_count++;
    }
  }

  // There should be at least one unload per truck
  EXPECT_GT(unload_count, num_trucks);

  // Each truck should have mined at least once
  for (const auto& count : mine_count) {
    EXPECT_GT(count, 1);
  }
}

// Verifies that trucks unload in the same order they finish mining
TEST_P(TestController_WithParams, UnloadOrderMatchesMiningOrder) {
  auto& events = GetEventLogger().GetEvents();
  events.clear();
  ASSERT_EQ(events.size(), 0);  // Sanity check: logger cleared

  Controller controller(/*num_trucks=*/10, /*num_stations=*/1);
  controller.Run(72 * 60min);  // Simulate full 72-hour window

  ASSERT_GT(events.size(), 0);  // Sanity check: events produced

  MinHeap mining_order;     // Ordered by mining end time
  MinHeap unloading_order;  // Ordered by unloading start time

  // Collect the order in which trucks finished mining and started unloading
  for (auto event = events.begin(); event != events.end(); ++event) {
    if (event->type == EventType::Mine)
      mining_order.push({event->end_time, event->truck_id});
    if (event->type == EventType::Unload)
      unloading_order.push({event->start_time, event->truck_id});
  }

  // Verify that trucks unload in the same order they finished mining
  while (!mining_order.empty() && !unloading_order.empty()) {
    ASSERT_EQ(mining_order.take().second, unloading_order.take().second);
  }
}

// Verifies that trucks and stations are never scheduled with overlapping
// events.
TEST_P(TestController_WithParams, TimesDoNotOverlap) {
  size_t num_trucks = 15;
  size_t num_stations = 4;
  auto& events = GetEventLogger().GetEvents();
  events.clear();
  ASSERT_EQ(events.size(), 0);  // Sanity check: logger cleared

  Controller controller(num_trucks, num_stations);
  controller.Run(10 * 60min);

  std::vector<minutes_t> truck_times(num_trucks, 0min);
  std::vector<minutes_t> station_times(num_stations, 0min);

  for (const auto& event : events) {
    if (event.type == EventType::Unload && event.station_id) {
      auto sid = event.station_id.value();
      ASSERT_GE(event.start_time, station_times[sid]);
      station_times[sid] = event.end_time;
    }

    // Queue events can overalp
    if (event.type != EventType::Queue) {
      ASSERT_GE(event.start_time, truck_times[event.truck_id]);
      truck_times[event.truck_id] = event.end_time;
    }
  }
}

// Ensures no event starts or ends beyond the simulation time
TEST_P(TestController_WithParams, NoEventsExceedSimTime) {
  const auto& params = GetParam();
  Controller controller(params.num_trucks, params.num_stations);
  controller.Run(params.sim_time);
  auto& events = GetEventLogger().GetEvents();
  for (const auto& event : events) {
    EXPECT_LE(event.start_time, params.sim_time)
        << "Event starts after sim_time: " << event;
    EXPECT_LE(event.end_time, params.sim_time)
        << "Event ends after sim_time: " << event;
  }
}
