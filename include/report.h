#ifndef INCLUDE_REPORT_H_
#define INCLUDE_REPORT_H_

#include <stddef.h>  // size_t

#include <utility>
#include <vector>

#include "minutes.h"

// Aggregated performance stats for a single truck
struct TruckMetrics {
  double utilization;        // % of time the truck was doing useful work
  minutes_t idle_time;       // Total time spent waiting (e.g., in queue)
  size_t trips_completed;    // Full mine -> travel -> unload cycles completed
  size_t mines_completed;    // Number of mining operations completed
  size_t queues_completed;   // Number of times the truck waited in queue
  minutes_t mining_time;     // Total time spent mining
  minutes_t queueing_time;   // Total time spent waiting to unload
  double avg_trip_time;      // Mean duration per full cycle
  double avg_queueing_time;  // Average wait time per queue event
};

// Aggregated performance stats for a single unload station
struct StationMetrics {
  double utilization;        // % of time the station was unloading trucks
  minutes_t idle_time;       // Total time not unloading
  size_t throughput;         // Number of trucks unloaded
  size_t queues_completed;   // Number of trucks that queued at this station
  minutes_t unloading_time;  // Total time spent unloading
  minutes_t queueing_time;   // Total time trucks waited for this station
  double avg_queueing_time;  // Average wait time per truck
};

// Calculates and returns metrics for all trucks and stations
std::pair<std::vector<TruckMetrics>, std::vector<StationMetrics>>
GenerateMetrics(minutes_t sim_time, size_t num_trucks, size_t num_stations);

// Prints the summary metrics to stdout in a readable format
void PrintMetricsSummary(const std::vector<TruckMetrics>& trucks,
                         const std::vector<StationMetrics>& stations,
                         minutes_t sim_time);

// Exports a formatted report of truck and station metrics to JSON
void ExportMetricsToJson(const std::vector<TruckMetrics>& trucks,
                         const std::vector<StationMetrics>& stations,
                         minutes_t sim_time);

// Serializes all events to a JSON file (e.g., for debugging or analysis)
void ExportAllEventsToJson(size_t num_trucks, size_t num_stations,
                           minutes_t sim_time);

#endif  // INCLUDE_REPORT_H_
