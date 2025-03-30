#ifndef INCLUDE_STATION_MANAGER_H_
#define INCLUDE_STATION_MANAGER_H_

#include <stddef.h>  // size_t

#include <functional>
#include <queue>
#include <utility>
#include <vector>

#include "minutes.h"

// Manages station availability
class StationManager {
 public:
  static constexpr minutes_t kUnloadTime = 5min;

  explicit StationManager(size_t count);

  std::pair<minutes_t, size_t> NextAvailableStation() const;
  minutes_t UnloadTruck(size_t truck_id, minutes_t arrival_time);

 private:
  std::priority_queue<std::pair<minutes_t, size_t>,
                      std::vector<std::pair<minutes_t, size_t>>, std::greater<>>
      queue_;
};

#endif  // INCLUDE_STATION_MANAGER_H_
