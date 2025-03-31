#include "report.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

#include "controller.h"
#include "event.h"
#include "station_manager.h"

using json = nlohmann::json;

std::pair<std::vector<TruckMetrics>, std::vector<StationMetrics>>
GenerateMetrics(minutes_t sim_time, size_t num_trucks, size_t num_stations) {
  std::vector<TruckMetrics> truck_metrics(num_trucks);
  std::vector<StationMetrics> station_metrics(num_stations);

  for (const auto event : GetEventLogger().GetEvents()) {
    const auto duration = event.end_time - event.start_time;
    switch (event.type) {
      case EventType::Mine:
        truck_metrics[event.truck_id].mining_time += duration;
        truck_metrics[event.truck_id].mines_completed++;
        break;
      case EventType::Unload:
        truck_metrics[event.truck_id].trips_completed++;
        station_metrics[event.station_id.value()].throughput++;
        break;
      case EventType::Queue:
        truck_metrics[event.truck_id].queueing_time += duration;
        truck_metrics[event.truck_id].queues_completed++;
        station_metrics[event.station_id.value()].queueing_time += duration;
        station_metrics[event.station_id.value()].queues_completed++;
        break;
      default:
        break;
    }
  }

  for (auto& truck : truck_metrics) {
    truck.avg_mining_time =
        static_cast<double>(truck.mining_time.count()) / truck.mines_completed;
    if (truck.queues_completed) {
      truck.avg_queueing_time =
          static_cast<double>(truck.queueing_time.count()) /
          truck.queues_completed;
    }
    truck.idle_time = truck.queueing_time;
    truck.utilization =
        static_cast<double>((sim_time - truck.idle_time).count()) /
        static_cast<double>(sim_time.count()) * 100.0;
  }

  for (auto& station : station_metrics) {
    if (station.queues_completed) {
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

// Prints detailed truck and station statistics reports to console.
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
              << "  Avg Mining Time: " << std::fixed << std::setprecision(2)
              << metric.avg_mining_time << " min\n"
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

  std::ofstream file("events.json");
  file << std::setw(2) << out << std::endl;  // Pretty print the JSON file
}
