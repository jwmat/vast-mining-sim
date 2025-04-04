#include "controller.h"

#include <algorithm>
#include <cassert>

#include "logger.h"

void StationQueue::Initialize(size_t num_stations) {
  for (size_t i = 0; i < num_stations; ++i) {
    queue_.emplace(0min, i);
  }
}

bool StationQueue::Empty() const { return queue_.empty(); }

std::pair<minutes_t, size_t> StationQueue::PopNextAvailable() {
  auto entry = queue_.top();
  queue_.pop();
  return entry;
}

void StationQueue::MarkAvailable(minutes_t time, size_t station_id) {
  queue_.emplace(time, station_id);
}

// Constructor initializes number of trucks, stations, and RNG seed
Controller::Controller(size_t num_trucks, size_t num_stations,
                       size_t random_seed)
    : num_trucks_(num_trucks),
      num_stations_(num_stations),
      engine_(random_seed) {}

// Utility to create, log, and enqueue an event
void Controller::EmitEvent(EventType type, size_t truck_id,
                           std::optional<size_t> station_id, minutes_t start,
                           minutes_t end) {
  const Event event{type, truck_id, station_id, start, end};
  LogEvent(event);
  event_queue_.push({end, event});
}

void Controller::Run(minutes_t sim_time) {
  if (num_trucks_ == 0 || num_stations_ == 0) {
    Logger::LogError("No trucks or stations.");
    return;
  }

  sim_duration_ = sim_time;
  trucks_metrics_.assign(num_trucks_, {});
  station_metrics_.assign(num_stations_, {});
  station_queue_.Initialize(num_stations_);

  // Dispatch all trucks to start mining
  for (size_t i = 0; i < num_trucks_; i++) {
    Mine(i, 0min);
  }

  // Main simulation loop: handle events until no more remain
  while (!event_queue_.empty()) {
    const auto [start_time, previous_event] = event_queue_.top();
    event_queue_.pop();
    ProcessEvent(start_time, previous_event);
  }

  // Collect and export simulation metrics
  GenerateMetrics(sim_time, &trucks_metrics_, &station_metrics_);
  ExportMetricsToJson(sim_time, trucks_metrics_, station_metrics_);
}

// Handle a single simulation event by delegating to the appropriate transition
void Controller::ProcessEvent(minutes_t start_time, const Event& event) {
  assert(start_time == event.end_time);
  const auto truck_id = event.truck_id;
  switch (event.type) {
    case EventType::Mine: {
      TravelToStation(truck_id, start_time);
      break;
    }
    case EventType::TravelToStation: {
      if (station_queue_.Empty()) {
        Logger::LogTrace("Station queue is empty!");
        break;
      }
      UnloadTruck(truck_id, start_time);
      break;
    }
    case EventType::Unload: {
      TravelToMine(truck_id, start_time);
      break;
    }
    case EventType::TravelToMine: {
      Mine(truck_id, start_time);
      break;
    }
    default: {
      Logger::LogError("Unrecognized event type!");
      break;
    }
  }
}

// Check if a time is beyond the simulation limit, and log if so
bool Controller::ExceedsSimTime(minutes_t time) const {
  if (time <= sim_duration_) return false;
  Logger::LogTrace(
      "[Time Limit Exceeded] Time: " + std::to_string(time.count()) +
      ", Limit: " + std::to_string(sim_duration_.count()));
  return true;
}

// Generate a random mining duration within a fixed range
minutes_t Controller::RandomMiningDuration() {
  std::uniform_int_distribution<uint64_t> dist(kMinDuration.count(),
                                               kMaxDuration.count());
  return minutes_t(dist(engine_));
}

// Schedule the truck to travel from mine to station
void Controller::TravelToStation(size_t truck_id, minutes_t start_time) {
  const auto end_time = start_time + kTravelTime;
  if (!ExceedsSimTime(end_time)) {
    EmitEvent(EventType::TravelToStation, truck_id, std::nullopt, start_time,
              end_time);
    trucks_metrics_[truck_id].travel_time += kTravelTime;
  }
}

// Record that the truck waited in line at a station
void Controller::RecordQueueing(size_t truck_id, size_t station_id,
                                minutes_t start_time, minutes_t end_time) {
  EmitEvent(EventType::Queue, truck_id, station_id, start_time, end_time);
  const auto duration = end_time - start_time;
  trucks_metrics_[truck_id].queueing_time += duration;
  trucks_metrics_[truck_id].queues_completed++;
  station_metrics_[station_id].queueing_time += duration;
  station_metrics_[station_id].queues_completed++;
}

// Schedule the truck to unload at a station
void Controller::UnloadTruck(size_t truck_id, minutes_t start_time) {
  const auto [available_time, station_id] = station_queue_.PopNextAvailable();

  // If the truck arrives before the station is available, track wait time
  const auto end_time = std::max(start_time, available_time) + kUnloadTime;
  if (!ExceedsSimTime(end_time)) {
    if (available_time > start_time) {
      RecordQueueing(truck_id, station_id, start_time, available_time);
    }

    start_time = std::max(available_time, start_time);
    const auto end_time = start_time + kUnloadTime;
    EmitEvent(EventType::Unload, truck_id, station_id, start_time, end_time);
    station_queue_.MarkAvailable(end_time, station_id);

    // Update metrics
    trucks_metrics_[truck_id].trips_completed++;
    trucks_metrics_[truck_id].unloading_time += kUnloadTime;
    station_metrics_[station_id].throughput++;
    station_metrics_[station_id].unloading_time += kUnloadTime;
  }
}

// Schedule the truck to return to the mine
void Controller::TravelToMine(size_t truck_id, minutes_t start_time) {
  const auto end_time = start_time + kTravelTime;
  if (!ExceedsSimTime(end_time)) {
    EmitEvent(EventType::TravelToMine, truck_id, std::nullopt, start_time,
              end_time);
    trucks_metrics_[truck_id].travel_time += kTravelTime;
  }
}

// Schedule the truck to mine again
void Controller::Mine(size_t truck_id, minutes_t start_time) {
  const auto duration = RandomMiningDuration();
  const auto end_time = start_time + duration;
  if (!ExceedsSimTime(end_time)) {
    EmitEvent(EventType::Mine, truck_id, std::nullopt, start_time, end_time);
    trucks_metrics_[truck_id].mines_completed++;
    trucks_metrics_[truck_id].mining_time += duration;
  }
}
