#include "truck_manager.h"

#include <utility>

#include "event.h"
#include "logger.h"

TruckManager::TruckManager(size_t count) {
  for (size_t i = 0; i < count; i++) {
    DispatchTruckToMine(i, 0min, 0min);
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

minutes_t TruckManager::DispatchTruckToMine(size_t truck_id,
                                            minutes_t start_time,
                                            minutes_t mine_time) {
  auto end_time = start_time + mine_time;
  LogEvent({EventType::Mine, truck_id, std::nullopt, start_time, end_time});
  queue_.push({end_time, truck_id});
  return end_time;
}
