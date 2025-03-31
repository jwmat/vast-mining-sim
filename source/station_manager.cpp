#include "station_manager.h"

#include <algorithm>
#include <utility>

#include "event.h"
#include "logger.h"

// Initialize all stations as available at time 0
StationManager::StationManager(size_t count) {
  for (size_t i = 0; i < count; i++) {
    queue_.push({0min, i});
  }
}

// Returns the earliest available station without removing it
MinHeap::type StationManager::NextAvailableStation() const {
  if (queue_.empty()) {
    LogAndThrowError<std::runtime_error>("No stations available.");
  }
  return queue_.top();
}

// Schedules a truck to unload at a station and returns its completion time
MinHeap::type StationManager::UnloadTruck(size_t truck_id,
                                          minutes_t arrival_time) {
  auto [available_time, station_id] = queue_.take();

  // Log queuing event if the truck arrives before the station is ready
  if (available_time - arrival_time > 0min) {
    LogEvent(
        {EventType::Queue, truck_id, station_id, arrival_time, available_time});
  }

  // Start unloading when station is free (or truck arrives later)
  const auto start_time = std::max(available_time, arrival_time);
  const auto end_time = start_time + kUnloadTime;
  LogEvent({EventType::Unload, truck_id, station_id, start_time, end_time});

  // Reinsert the station with its next available time
  queue_.push({end_time, station_id});
  return {end_time, station_id};
}
