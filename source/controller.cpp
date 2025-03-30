#include "controller.h"

Controller::Controller(size_t num_stations, size_t num_trucks)
    : station_manager_(std::make_unique<StationManager>(num_stations)),
      truck_manager_(std::make_unique<TruckManager>(num_trucks)) {}

void Controller::Run(minutes_t sim_time) {
  // Initial dispatch of all trucks to mines ensures the truck finished mining
  // first is dispatched to the station first
  const auto num_trucks = truck_manager_->TrucksAvailable();
  const auto start_time = 0min;
  for (size_t i = 0; i < num_trucks; i++) {
    const auto mine_time = Random::Duration();
    if (ExceedsSimTime(start_time, mine_time, sim_time)) continue;
    truck_manager_->DispatchTruckToMine(i, start_time, mine_time);
  }

  // We don't want to end the simulation when the first truck hits the time
  // limit, so we will check to make sure the truck is only placed back into
  // the queue if it can complete the next phase
  while (truck_manager_->TrucksAvailable() > 0) {
    // Check if the truck can travel to the station
    auto [start_time, truck_id] = truck_manager_->NextAvailableTruck();
    if (ExceedsSimTime(start_time, kTravelTime, sim_time)) continue;

    // Put truck on the road
    LogEvent({EventType::Travel, truck_id, std::nullopt, start_time,
              start_time + kTravelTime});
    start_time += kTravelTime;

    // Check if the truck can unload
    if (ExceedsSimTime(start_time, StationManager::kUnloadTime, sim_time))
      continue;
    auto [_, station_id] = station_manager_->NextAvailableStation();

    // Unload the truck
    auto unload_time =
        station_manager_->UnloadTruck(truck_id, start_time + kTravelTime);

    // Check if truck can travel back to the mine
    if (ExceedsSimTime(unload_time, kTravelTime, sim_time)) continue;

    // Put truck on the road
    LogEvent({EventType::Travel, truck_id, std::nullopt, unload_time,
              unload_time + kTravelTime});
    start_time = unload_time + kTravelTime;

    // Check if truck can finish another mine
    auto mine_time = Random::Duration();
    if (ExceedsSimTime(start_time, mine_time, sim_time)) continue;

    // Truck mines
    truck_manager_->DispatchTruckToMine(truck_id, start_time, mine_time);
  }
}

bool ControllerExceedsSimTime(minutes_t start_time, minutes_t duration,
                              minutes_t sim_time) {
  return start_time + duration > sim_time;
}

const StationManager Controller::station_manager() { return *station_manager_; }
const TruckManager& Controller::truck_manager() { return *truck_manager_; }
