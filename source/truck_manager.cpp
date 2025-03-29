#include "truck_manager.h"

#include <utility>

TruckManager::TruckManager(size_t count) {
  for (size_t i = 0; i < count; i++) {
    queue_.push({0min, i});
  }
}

size_t TruckManager::TrucksAvailable() const { return queue_.size(); }

std::pair<minutes_t, size_t> TruckManager::NextAvailableTruck() {
  if (queue_.empty()) {
    throw std::runtime_error("No trucks available.");
  }
  auto next = queue_.top();
  queue_.pop();
  return next;
}

void TruckManager::DispatchToMine(size_t id, minutes_t time,
                                  minutes_t duration) {
  queue_.push({time + duration + kTravelTime, id});
}
