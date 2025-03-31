#include "report.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

#include "controller.h"
#include "event.h"
#include "logger.h"
#include "station_manager.h"

using json = nlohmann::json;

// Computes performance metrics for all trucks and stations
std::pair<std::vector<TruckMetrics>, std::vector<StationMetrics>>
GenerateMetrics(minutes_t sim_time, size_t num_trucks, size_t num_stations) {
  std::vector<TruckMetrics> truck_metrics(num_trucks);
  std::vector<StationMetrics> station_metrics(num_stations);

  // Accumulate values from all logged events
  for (const auto event : GetEventLogger().GetEvents()) {
    const auto duration = event.end_time - event.start_time;
    switch (event.type) {
      case EventType::Mine:
        truck_metrics[event.truck_id].mining_time += duration;
        truck_metrics[event.truck_id].mines_completed++;
        break;
      case EventType::Unload:
        truck_metrics[event.truck_id].trips_completed++;
        if (event.station_id) {
          station_metrics[event.station_id.value()].throughput++;
        } else {
          LogError("Missing station_id for event type " +
                   EventTypeToString(event.type));
        }
        break;
      case EventType::Queue:
        truck_metrics[event.truck_id].queueing_time += duration;
        if (event.station_id) {
          station_metrics[event.station_id.value()].queueing_time += duration;
          station_metrics[event.station_id.value()].queues_completed++;
        } else {
          LogError("Missing station_id for event type " +
                   EventTypeToString(event.type));
        }
        break;
      default:
        break;
    }
  }

  // Finalize computed metrics for each truck
  for (auto& truck : truck_metrics) {
    truck.idle_time = sim_time - truck.mining_time -
                      truck.trips_completed * Controller::kTravelTime -
                      (truck.mines_completed - 1) * Controller::kTravelTime -
                      truck.trips_completed * StationManager::kUnloadTime;
    if (truck.trips_completed > 0) {
      truck.avg_trip_time =
          static_cast<double>((sim_time - truck.idle_time).count()) /
          truck.trips_completed;
    }
    if (truck.queues_completed > 0) {
      truck.avg_queueing_time =
          static_cast<double>(truck.queueing_time.count()) /
          truck.queues_completed;
    }
    truck.utilization =
        static_cast<double>((sim_time - truck.idle_time).count()) /
        static_cast<double>(sim_time.count()) * 100.0;
  }

  // Finalize computed metrics for each station
  for (auto& station : station_metrics) {
    if (station.queues_completed > 0) {
      station.avg_queueing_time =
          static_cast<double>(station.queueing_time.count()) /
          station.queues_completed;
    }
    station.unloading_time = station.throughput * StationManager::kUnloadTime;
    station.idle_time = sim_time - station.unloading_time;
    station.utilization = static_cast<double>(station.unloading_time.count()) /
                          static_cast<double>(sim_time.count()) * 100.0;
  }

  return {truck_metrics, station_metrics};
}

// Prints a formatted report of truck and station metrics to the console
void PrintMetrics(
    std::pair<std::vector<TruckMetrics>, std::vector<StationMetrics>> metrics) {
  std::cout << "\n=== Truck Stats ===\n";
  for (size_t i = 0; i < metrics.first.size(); i++) {
    auto metric = metrics.first[i];
    std::cout << "Truck " << i << ":\n"
              << "  Utilization: " << std::fixed << std::setprecision(2)
              << metric.utilization << "% \n"
              << "  Idle Time: " << metric.idle_time << "\n"
              << "  Trips Completed: " << metric.trips_completed << "\n"
              << "  Avg Trip Time: " << std::fixed << std::setprecision(2)
              << metric.avg_trip_time << "\n"
              << "  Avg Queueing Time: " << std::fixed << std::setprecision(2)
              << metric.avg_queueing_time << "\n";
  }

  std::cout << "\n=== Station Stats ===\n";
  for (size_t i = 0; i < metrics.second.size(); i++) {
    auto metric = metrics.second[i];
    std::cout << "Station " << i << ":\n"
              << "  Utilization: " << std::fixed << std::setprecision(2)
              << metric.utilization << "% \n"
              << "  Idle Time: " << metric.idle_time << "\n"
              << "  Throughput: " << metric.throughput << "\n"
              << "  Avg Queueing Time: " << std::fixed << std::setprecision(2)
              << metric.avg_queueing_time << "\n";
  }
}

// Exports all events to a JSON file for analysis or visualization
void ExportAllEventsToJson(minutes_t sim_time) {
  json out;

  out["simulation_duration"] = sim_time.count();

  // Serialize each event into JSON
  for (const auto& event : GetEventLogger().GetEvents()) {
    json event_json;
    event_json["type"] = EventTypeToString(event.type);

    event_json["truck_id"] = event.truck_id;
    event_json["start_time"] = event.start_time.count();
    event_json["end_time"] = event.end_time.count();

    if (event.station_id) {
      event_json["station_id"] = event.station_id.value();
    }

    out["events"].push_back(event_json);
  }

  // Write to file with pretty formatting
  std::ofstream file("events.json");
  file << std::setw(2) << out << std::endl;
}
