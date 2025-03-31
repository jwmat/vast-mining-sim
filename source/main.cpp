#include <iostream>
#include <sstream>

#include "controller.h"
#include "logger.h"
#include "minutes.h"

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

  return 0;
}
