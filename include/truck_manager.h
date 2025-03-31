#ifndef INCLUDE_TRUCK_MANAGER_H_
#define INCLUDE_TRUCK_MANAGER_H_

#include <stddef.h>  // size_t

#include <vector>

#include "event.h"
#include "heap.h"
#include "minutes.h"

// Manages availability and dispatching of mining trucks
class TruckManager {
 public:
  explicit TruckManager(size_t count);

  // Returns the number of trucks currently available
  size_t TrucksAvailable() const;

  // Removes and returns the next available truck from the queue
  MinHeap::type DispatchTruck();

  // Returns a truck to the queue with its next available time
  void ReturnTruck(size_t truck_id, minutes_t time);

 private:
  MinHeap queue_;                 // Tracks trucks by their availability time
  std::vector<bool> dispatched_;  // Tracks trucks by their availability time
};

#endif  // INCLUDE_TRUCK_MANAGER_H_
