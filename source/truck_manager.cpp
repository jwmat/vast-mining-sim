#include "truck_manager.h"

#include <utility>

#include "event.h"
#include "logger.h"

// Initializes all trucks as available at time 0
TruckManager::TruckManager(size_t count) {
  dispatched_.resize(count, false);
  for (size_t i = 0; i < count; i++) {
    queue_.push({0min, i});
  }
}

size_t TruckManager::TrucksAvailable() const { return queue_.size(); }

// Dispatches the next available truck and marks it as in-use
MinHeap::type TruckManager::DispatchTruck() {
  if (queue_.empty()) {
    LogAndThrowError<std::runtime_error>("No trucks left in queue.");
  }
  auto top = queue_.take();
  dispatched_[top.second] = true;
  return top;
}

// Returns a truck to the queue after completing its task
void TruckManager::ReturnTruck(size_t truck_id, minutes_t time) {
  if (dispatched_[truck_id] == false) {
    LogAndThrowError<std::runtime_error>("Truck was not dispatched.");
  }
  dispatched_[truck_id] = false;
  queue_.push({time, truck_id});
}
