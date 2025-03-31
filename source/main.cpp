#include <iostream>
#include <sstream>

#include "controller.h"
#include "logger.h"
#include "minutes.h"
#include "report.h"

constexpr minutes_t kMaxSimTime = 72 * 60min;

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::ostringstream message;
    LogAndThrowError<std::invalid_argument>(
        "Expected input arguments: <num_trucks> <num_stations>");
  }

  size_t num_trucks = std::stoul(argv[1]);
  size_t num_stations = std::stoul(argv[2]);

  if (num_trucks == 0 || num_stations == 0) {
    LogAndThrowError<std::invalid_argument>(
        "Must provide at least one truck and one station.");
  }

  Controller controller(num_trucks, num_stations);
  controller.Run(kMaxSimTime);

  auto metrics = GenerateMetrics(kMaxSimTime, num_trucks, num_stations);
  PrintMetrics(metrics);
  ExportAllEventsToJson(kMaxSimTime);

  return 0;
}
