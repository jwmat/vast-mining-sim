#ifndef INCLUDE_STATION_MANAGER_H_
#define INCLUDE_STATION_MANAGER_H_

#include <stddef.h>  // size_t

#include "event.h"
#include "heap.h"
#include "minutes.h"

// Manages a set of unload stations and their availability over time
class StationManager {
 public:
  static constexpr minutes_t kUnloadTime = 5min;  // Time to unload at a station

  explicit StationManager(size_t count);

  // Returns the next available station (peek only)
  MinHeap::type NextAvailableStation() const;

  // Assigns a truck to unload at a station, logging any wait and the unload
  // event
  MinHeap::type UnloadTruck(size_t truck_id, minutes_t arrival_time);

 private:
  MinHeap queue_;  // Min-heap by station availability time
};

#endif  // INCLUDE_STATION_MANAGER_H_
