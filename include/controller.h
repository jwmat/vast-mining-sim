#ifndef INCLUDE_CONTROLLER_H_
#define INCLUDE_CONTROLLER_H_

#include <functional>
#include <memory>
#include <queue>
#include <random>
#include <utility>
#include <vector>

#include "event.h"
#include "report.h"

// StationQueue manages station availability scheduling using a min-heap
class StationQueue {
 public:
  void Initialize(size_t num_stations);

  bool Empty() const;
  std::pair<minutes_t, size_t> PopNextAvailable();
  void MarkAvailable(minutes_t time, size_t station_id);

 private:
  std::priority_queue<std::pair<minutes_t, size_t>,
                      std::vector<std::pair<minutes_t, size_t>>, std::greater<>>
      queue_;
};

// Controls the simulation by coordinating truck, mine, and station behavior.
// Owns the main loop and delegates work to handlers per event type.
class Controller {
 public:
  // Constants controlling simulation timing
  static constexpr minutes_t kTravelTime = 30min;
  static constexpr minutes_t kUnloadTime = 5min;
  static constexpr minutes_t kMinDuration = 60min;
  static constexpr minutes_t kMaxDuration = 300min;

  Controller(size_t num_trucks, size_t num_stations,
             size_t random_seed = 0xBEEF);

  // Runs the simulation for the given amount of simulated time (in minutes)
  void Run(minutes_t sim_time);

 private:
  // Event processing entry point
  void ProcessEvent(minutes_t start_time, const Event& event);

  // Core simulation transitions
  void Mine(size_t truck_id, minutes_t start_time);
  void TravelToStation(size_t truck_id, minutes_t start_time);
  void UnloadTruck(size_t truck_id, minutes_t start_time);
  void TravelToMine(size_t truck_id, minutes_t start_time);

  // Queue tracking and metric recording
  void RecordQueueing(size_t truck_id, size_t station_id, minutes_t start_time,
                      minutes_t end_time);

  // Utility to create/log/queue a simulation event
  void EmitEvent(EventType type, size_t truck_id,
                 std::optional<size_t> station_id, minutes_t start,
                 minutes_t end);

  // Support functions
  bool ExceedsSimTime(minutes_t time) const;
  minutes_t RandomMiningDuration();

  // Configuration and state
  size_t num_trucks_ = 0;
  size_t num_stations_ = 0;
  minutes_t sim_duration_ = 0min;
  std::default_random_engine engine_;

  // Scheduling and event management
  std::priority_queue<std::pair<minutes_t, Event>,
                      std::vector<std::pair<minutes_t, Event>>, std::greater<>>
      event_queue_;
  StationQueue station_queue_;

  // Metrics for trucks and stations
  std::vector<TruckMetrics> trucks_metrics_;
  std::vector<StationMetrics> station_metrics_;
};

#endif  // INCLUDE_CONTROLLER_H_
