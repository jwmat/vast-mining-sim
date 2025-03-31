#ifndef INCLUDE_CONTROLLER_H_
#define INCLUDE_CONTROLLER_H_

#include <memory>
#include <utility>

#include "event.h"
#include "mine_manager.h"
#include "station_manager.h"
#include "truck_manager.h"

// Controls the simulation by coordinating truck, mine, and station behavior.
// Owns the main loop and delegates work to managers.
class Controller {
 public:
  static constexpr minutes_t kTravelTime =
      30min;  // Fixed travel time between mine and station

  Controller(size_t num_trucks, size_t num_stations);

  // Runs the simulation for the given amount of simulated time (in minutes)
  void Run(minutes_t sim_time);

  const MineManager& mine_manager();
  const StationManager station_manager();
  const TruckManager& truck_manager();

 private:
  std::unique_ptr<MineManager> mine_manager_;
  std::unique_ptr<StationManager> station_manager_;
  std::unique_ptr<TruckManager> truck_manager_;

  // Returns true if the operation starting at start_time with given duration
  // would exceed sim_time
  bool ExceedsSimTime(minutes_t start_time, minutes_t duration,
                      minutes_t sim_time);
};

#endif  // INCLUDE_CONTROLLER_H_
