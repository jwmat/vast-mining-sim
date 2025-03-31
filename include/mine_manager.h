#ifndef INCLUDE_MINE_MANAGER_H_
#define INCLUDE_MINE_MANAGER_H_

#define SEED 0xBEEF

#include <queue>
#include <random>
#include <vector>

#include "event.h"
#include "minutes.h"

// Manages mining operations for trucks.
// Currently assumes infinite mining site availability and no contention.
// This class is designed to be extensible for future scenarios where mines
// are finite and require dispatching or queuing.
class MineManager {
 public:
  // Minimum and maximum durations for a mining operation
  static constexpr minutes_t kMinDuration = 60min;
  static constexpr minutes_t kMaxDuration = 300min;

  MineManager(/*size_t num_mines*/);

  // Logs a mining event and returns the created Event by reference
  const Event& MineTruck(size_t truck_id, minutes_t start_time,
                         minutes_t mine_time);

  // Returns a random mining duration between min and max bounds
  static minutes_t Duration();

 private:
  static std::default_random_engine engine_;

  // Placeholder for future mine assignment/queueing logic
  // std::priority_queue<std::pair<minutes_t, size_t>,
  //                     std::vector<std::pair<minutes_t, size_t>>,
  //                     std::greater<>>
  //     queue_;
};

#endif  // INCLUDE_MINE_MANAGER_H_
