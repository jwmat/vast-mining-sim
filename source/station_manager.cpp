#include "station_manager.h"

#include <algorithm>
#include <utility>

#include "event.h"

StationManager::StationManager(size_t count) {
  for (size_t i = 0; i < count; i++) {
    queue_.push({0min, i});
  }
}

std::pair<minutes_t, size_t> StationManager::NextAvailableStation() const {
  if (queue_.empty()) {
    throw std::runtime_error("No stations available.");
  }
  return queue_.top();
}

minutes_t StationManager::UnloadTruck(size_t truck_id, minutes_t arrival_time) {
  auto [available_time, station_id] = queue_.top();
  queue_.pop();

  // Queue the truck if the station isn't available
  if (available_time - arrival_time > 0min) {
    LogEvent(
        {EventType::Queue, truck_id, station_id, arrival_time, available_time});
  }

  const auto start_time = std::max(available_time, arrival_time);
  const auto end_time = start_time + kUnloadTime;
  LogEvent({EventType::Unload, truck_id, station_id, start_time, end_time});

  queue_.push({end_time, station_id});
  return end_time;
}
