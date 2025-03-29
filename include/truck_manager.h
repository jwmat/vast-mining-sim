#ifndef INCLUDE_TRUCK_MANAGER_H_
#define INCLUDE_TRUCK_MANAGER_H_

#include <stddef.h>  // size_t

#include <chrono>  // NOLINT(build/c++11)
#include <functional>
#include <queue>
#include <utility>
#include <vector>

using minutes_t = std::chrono::minutes;
using namespace std::chrono_literals;

// Manages truck availability
class TruckManager {
 public:
  static constexpr minutes_t kTravelTime = 30min;

  explicit TruckManager(size_t count);

  size_t TrucksAvailable() const;
  std::pair<minutes_t, size_t> NextAvailableTruck();
  void DispatchToMine(size_t id, minutes_t time, minutes_t duration);

 private:
  std::priority_queue<std::pair<minutes_t, size_t>,
                      std::vector<std::pair<minutes_t, size_t>>, std::greater<>>
      queue_;
};

#endif  // INCLUDE_TRUCK_MANAGER_H_
