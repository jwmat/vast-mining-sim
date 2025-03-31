#ifndef INCLUDE_REPORT_H_
#define INCLUDE_REPORT_H_

#include <stddef.h>  // size_t

#include <utility>
#include <vector>

#include "minutes.h"

struct TruckMetrics {
  double utilization;   // % of time the truck was doing useful work (mining,
                        // traveling, unloading) vs total simulation time
  minutes_t idle_time;  // Total time the truck was waiting (e.g., in queue)
  size_t
      trips_completed;  // Number of full mine->travel->unload cycles completed
  size_t mines_completed;    // Number of times mined
  size_t queues_completed;   // Number of times queued;
  minutes_t mining_time;     // Total duration spent mining per cycle
  minutes_t queueing_time;   // Total time spent queued at a station across all
                             // unload events
  double avg_mining_time;    // Mean duration spent mining per cycle
  double avg_queueing_time;  // Mean ime spent queued at a station across all
                             // unload events
};

struct StationMetrics {
  double utilization;        // % of time the station was busy unloading a truck
  minutes_t idle_time;       // Time not unloading (available but unused)
  size_t throughput;         // Number of trucks processed (i.e., unloaded)
  size_t queues_completed;   // Number of times queued;
  minutes_t unloading_time;  // Total time spent unloading;
  minutes_t queueing_time;   // Total time trucks waited to use this station
  double avg_queueing_time;  // Average time trucks waited to use this station
};

std::pair<std::vector<TruckMetrics>, std::vector<StationMetrics>>
GenerateMetrics(minutes_t sim_time, size_t num_trucks, size_t num_stations);

void PrintMetrics(
    std::pair<std::vector<TruckMetrics>, std::vector<StationMetrics>> metrics);

void ExportAllEventsToJson(minutes_t sim_time);

#endif  // INCLUDE_REPORT_H_
