#ifndef INCLUDE_TRUCK_MANAGER_H_
#define INCLUDE_TRUCK_MANAGER_H_

#include <stddef.h>  // size_t

#include <functional>
#include <queue>
#include <utility>
#include <vector>

#include "minutes.h"

// Manages truck availability
class TruckManager {
 public:
  static constexpr minutes_t kTravelTime = 30min;

  explicit TruckManager(size_t count);

  size_t TrucksAvailable() const;
  std::pair<minutes_t, size_t> NextAvailableTruck();
  minutes_t DispatchTruckToMine(size_t truck_id, minutes_t start_time,
                                minutes_t mine_time);

 private:
  std::priority_queue<std::pair<minutes_t, size_t>,
                      std::vector<std::pair<minutes_t, size_t>>, std::greater<>>
      queue_;
};

#endif  // INCLUDE_TRUCK_MANAGER_H_
