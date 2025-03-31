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
                      (truck.mines_completed - 1) *  // No travel for first mine
                          Controller::kTravelTime -
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

// Exports a formatted report of truck and station metrics to JSON
void ExportMetricsToJson(const std::vector<TruckMetrics>& trucks,
                         const std::vector<StationMetrics>& stations,
                         minutes_t sim_time) {
  json j;
  j["simulation_duration"] = sim_time.count();

  for (size_t i = 0; i < trucks.size(); ++i) {
    const auto& t = trucks[i];
    j["trucks"].push_back({{"id", i},
                           {"utilization", t.utilization},
                           {"idle_time", t.idle_time.count()},
                           {"trips_completed", t.trips_completed},
                           {"mines_completed", t.mines_completed},
                           {"queues_completed", t.queues_completed},
                           {"mining_time", t.mining_time.count()},
                           {"queueing_time", t.queueing_time.count()},
                           {"avg_trip_time", t.avg_trip_time},
                           {"avg_queueing_time", t.avg_queueing_time}});
  }

  for (size_t i = 0; i < stations.size(); ++i) {
    const auto& s = stations[i];
    j["stations"].push_back({{"id", i},
                             {"utilization", s.utilization},
                             {"idle_time", s.idle_time.count()},
                             {"throughput", s.throughput},
                             {"queues_completed", s.queues_completed},
                             {"unloading_time", s.unloading_time.count()},
                             {"queueing_time", s.queueing_time.count()},
                             {"avg_queueing_time", s.avg_queueing_time}});
  }

  std::ostringstream os;
  os << "metrics." << trucks.size() << "truck_" << stations.size() << "station_"
     << sim_time << "_minutes.json";
  std::ofstream out(os.str());
  out << std::setw(2) << j << std::endl;

  PrintMetricsSummary(trucks, stations, sim_time);
}

void PrintMetricsSummary(const std::vector<TruckMetrics>& trucks,
                         const std::vector<StationMetrics>& stations,
                         minutes_t sim_time) {
  double avg_truck_util = 0.0;
  for (const auto& t : trucks) avg_truck_util += t.utilization;
  avg_truck_util /= trucks.size();

  double avg_station_util = 0.0;
  for (const auto& s : stations) avg_station_util += s.utilization;
  avg_station_util /= stations.size();

  std::cout << "\n=== Simulation Summary ===\n"
            << "Simulation Time: " << sim_time.count() << " minutes\n"
            << "Trucks: " << trucks.size() << "\n"
            << "Stations: " << stations.size() << "\n"
            << "Average Truck Utilization: " << std::fixed
            << std::setprecision(2) << avg_truck_util << "%\n"
            << "Average Station Utilization: " << std::fixed
            << std::setprecision(2) << avg_station_util << "%\n";
}

// Exports all events to a JSON file for analysis or visualization
void ExportAllEventsToJson(size_t num_trucks, size_t num_stations,
                           minutes_t sim_time) {
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

  std::ostringstream os;
  os << "events." << num_trucks << "_trucks_" << num_stations << "_stations_"
     << sim_time << ".json";

  std::cout << "\nDetailed metrics for individual trucks and stations can be "
               "found in "
            << os.str();

  // Write to file with pretty formatting
  std::ofstream file(os.str());
  file << std::setw(2) << out << std::endl;
}
