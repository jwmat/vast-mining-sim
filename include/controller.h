#ifndef INCLUDE_CONTROLLER_H_
#define INCLUDE_CONTROLLER_H_

#include <memory>
#include <utility>

#include "event.h"
#include "random.h"
#include "station_manager.h"
#include "truck_manager.h"

class Controller {
 public:
  static constexpr minutes_t kTravelTime = 30min;

  Controller(size_t num_stations, size_t num_trucks);

  void Run(minutes_t sim_time);
  const StationManager station_manager();
  const TruckManager& truck_manager();

 private:
  std::unique_ptr<StationManager> station_manager_;
  std::unique_ptr<TruckManager> truck_manager_;

  bool ExceedsSimTime(minutes_t start_time, minutes_t duration,
                      minutes_t sim_time);
};

#endif  // INCLUDE_CONTROLLER_H_
