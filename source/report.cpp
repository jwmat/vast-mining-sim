#include "report.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>

#include "controller.h"
#include "event.h"
#include "logger.h"

using json = nlohmann::json;

// Computes performance metrics for all trucks and stations
void GenerateMetrics(minutes_t sim_time,
                     std::vector<TruckMetrics>* truck_metrics,
                     std::vector<StationMetrics>* station_metrics) {
  // Compute derived truck metrics
  for (auto& t : *truck_metrics) {
    // Idle = time not spent mining, unloading, or traveling
    t.idle_time = sim_time - (t.mining_time + t.unloading_time + t.travel_time);

    if (t.trips_completed > 0) {
      t.avg_trip_time = static_cast<double>((sim_time - t.idle_time).count()) /
                        t.trips_completed;
    }

    if (t.queues_completed > 0) {
      t.avg_queueing_time =
          static_cast<double>(t.queueing_time.count()) / t.queues_completed;
    }

    t.utilization = static_cast<double>((sim_time - t.idle_time).count()) /
                    static_cast<double>(sim_time.count()) * 100.0;
  }

  // Compute derived station metrics
  for (auto& s : *station_metrics) {
    if (s.queues_completed > 0) {
      s.avg_queueing_time =
          static_cast<double>(s.queueing_time.count()) / s.queues_completed;
    }

    s.idle_time = sim_time - s.unloading_time;
    s.utilization = static_cast<double>(s.unloading_time.count()) /
                    static_cast<double>(sim_time.count()) * 100.0;
  }
}

// Exports a formatted report of truck and station metrics to JSON
void ExportMetricsToJson(minutes_t sim_time,
                         const std::vector<TruckMetrics>& trucks,
                         const std::vector<StationMetrics>& stations) {
  json j;
  j["simulation_duration"] = sim_time.count();

  for (size_t i = 0; i < trucks.size(); ++i) {
    const auto& t = trucks[i];
    j["trucks"].push_back({
        {"id", i},
        {"utilization", t.utilization},
        {"idle_time", t.idle_time.count()},
        {"trips_completed", t.trips_completed},
        {"mines_completed", t.mines_completed},
        {"queues_completed", t.queues_completed},
        {"mining_time", t.mining_time.count()},
        {"queueing_time", t.queueing_time.count()},
        {"avg_trip_time", t.avg_trip_time},
        {"avg_queueing_time", t.avg_queueing_time},
    });
  }

  for (size_t i = 0; i < stations.size(); ++i) {
    const auto& s = stations[i];
    j["stations"].push_back({
        {"id", i},
        {"utilization", s.utilization},
        {"idle_time", s.idle_time.count()},
        {"throughput", s.throughput},
        {"queues_completed", s.queues_completed},
        {"unloading_time", s.unloading_time.count()},
        {"queueing_time", s.queueing_time.count()},
        {"avg_queueing_time", s.avg_queueing_time},
    });
  }

  // Construct a descriptive filename for output
  std::ostringstream os;
  os << "metrics." << trucks.size() << "truck_" << stations.size() << "station_"
     << sim_time << "_minutes.json";

  std::ofstream out(os.str());
  out << std::setw(2) << j << std::endl;

  PrintMetricsSummary(trucks, stations, sim_time);
  std::cout << "\nFull metrics report: " << os.str() << std::endl;
}

// Prints a summary of overall utilization to the console
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
