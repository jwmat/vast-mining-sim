#include "controller.h"

#include <cassert>

#include "logger.h"

Controller::Controller(size_t num_trucks, size_t num_stations)
    : station_manager_(std::make_unique<StationManager>(num_stations)),
      truck_manager_(std::make_unique<TruckManager>(num_trucks)) {}

void Controller::Run(minutes_t sim_time) {
  const auto start_time = 0min;
  const auto num_trucks = truck_manager_->TrucksAvailable();

  // Step 1: Dispatch all trucks to begin mining
  for (size_t i = 0; i < num_trucks; i++) {
    auto [start_time, truck_id] = truck_manager_->DispatchTruck();
    const auto mine_time = MineManager::Duration();
    if (ExceedsSimTime(start_time, mine_time, sim_time)) continue;
    const auto [end_time, _] =
        mine_manager_->MineTruck(truck_id, start_time, mine_time);
    truck_manager_->ReturnTruck(truck_id, end_time);
  }

  // Step 2: Continue dispatching trucks through complete cycles until time runs
  // out
  while (truck_manager_->TrucksAvailable() > 0) {
    // Dispatch the truck to the station
    auto [current_time, truck_id] = truck_manager_->DispatchTruck();

    {  // Travel to station
      if (ExceedsSimTime(current_time, kTravelTime, sim_time)) continue;
      LogEvent({EventType::Travel, truck_id, std::nullopt, current_time,
                current_time + kTravelTime});
      current_time += kTravelTime;
    }

    const auto [available_time, station_id] =
        station_manager_->GetNextAvailableSlot(current_time);
    {  // Unload at station
      if (ExceedsSimTime(available_time, StationManager::kUnloadTime, sim_time))
        continue;
      auto [end_time, station_id] =
          station_manager_->UnloadTruck(truck_id, current_time);
      current_time = end_time;
    }

    {  // Travel back to the mine
      if (ExceedsSimTime(current_time, kTravelTime, sim_time)) continue;
      LogEvent({EventType::Travel, truck_id, station_id, current_time,
                current_time + kTravelTime});
      current_time += kTravelTime;
    }

    {  // Begin mining again
      const auto mine_time = MineManager::Duration();
      if (ExceedsSimTime(current_time, mine_time, sim_time)) continue;
      const auto [end_time, _] =
          mine_manager_->MineTruck(truck_id, current_time, mine_time);
      current_time = end_time;
    }

    // Return the truck to the queue
    truck_manager_->ReturnTruck(truck_id, current_time);
  }
}

bool Controller::ExceedsSimTime(minutes_t start_time, minutes_t duration,
                                minutes_t sim_time) {
  bool exceeds = start_time + duration > sim_time;
  if (exceeds) {
    std::ostringstream message;
    message << "[Time Limit Exceeded] Start Time: " << start_time
            << ", Duration: " << duration << ", Time Limit: " << sim_time;
    LogInfo(message.str());
  }

  return start_time + duration > sim_time;
}

const MineManager& Controller::mine_manager() { return *mine_manager_; }
const StationManager Controller::station_manager() { return *station_manager_; }
const TruckManager& Controller::truck_manager() { return *truck_manager_; }
