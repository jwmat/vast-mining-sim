#include "station_manager.h"

#include <algorithm>
#include <utility>

StationManager::StationManager(size_t count) {
  for (size_t i = 0; i < count; i++) {
    queue_.push({0min, i});
  }
}

std::pair<minutes_t, size_t> StationManager::NextAvailableStation() {
  if (queue_.empty()) {
    throw std::runtime_error("No stations available.");
  }
  auto next = queue_.top();
  queue_.pop();
  return next;
}

minutes_t StationManager::UnloadTruck(size_t id, minutes_t available_time,
                                      minutes_t arrival_time) {
  const auto finish_time = std::max(available_time, arrival_time) + kUnloadTime;
  queue_.push({finish_time, id});
  return finish_time;
}
