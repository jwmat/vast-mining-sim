#ifndef INCLUDE_REPORT_H_
#define INCLUDE_REPORT_H_

#include <stddef.h>  // size_t

#include <vector>

#include "minutes.h"

// Aggregated performance statistics for a single truck over the simulation.
struct TruckMetrics {
  double utilization = 0.0;     // % of simulation time doing useful work
  size_t trips_completed = 0;   // Full mine -> travel -> unload cycles
  size_t mines_completed = 0;   // Individual mining operations
  size_t queues_completed = 0;  // Number of times the truck queued

  minutes_t idle_time = 0min;       // Time not doing productive work
  minutes_t mining_time = 0min;     // Time spent mining
  minutes_t queueing_time = 0min;   // Time spent in unloading queues
  minutes_t unloading_time = 0min;  // Time spent unloading
  minutes_t travel_time = 0min;     // Time spent in transit

  double avg_trip_time = 0.0;      // Mean time per trip
  double avg_queueing_time = 0.0;  // Mean time spent queueing
};

// Aggregated performance statistics for a single unload station.
struct StationMetrics {
  double utilization = 0.0;     // % of time the station was unloading
  size_t throughput = 0;        // Number of trucks unloaded
  size_t queues_completed = 0;  // Queues served at this station

  minutes_t idle_time = 0min;       // Time not unloading
  minutes_t unloading_time = 0min;  // Cumulative unload duration
  minutes_t queueing_time = 0min;   // Time trucks spent waiting here

  double avg_queueing_time = 0.0;  // Mean wait time per truck
};

// Calculates simulation-wide truck and station metrics in-place.
void GenerateMetrics(minutes_t sim_time, std::vector<TruckMetrics>* trucks,
                     std::vector<StationMetrics>* stations);

// Outputs an overall summary (e.g., average utilization) to stdout.
void PrintMetricsSummary(const std::vector<TruckMetrics>& trucks,
                         const std::vector<StationMetrics>& stations,
                         minutes_t sim_time);

// Exports a detailed report to a JSON file (1 truck/station entry per object).
void ExportMetricsToJson(minutes_t sim_time,
                         const std::vector<TruckMetrics>& trucks,
                         const std::vector<StationMetrics>& stations);

// Optional: dump all raw events to a JSON file (not used in your main code).
void ExportAllEventsToJson(size_t num_trucks, size_t num_stations,
                           minutes_t sim_time);

#endif  // INCLUDE_REPORT_H_
