#include <chrono>  // NOLINT(build/c++11)
#include <cstdlib>
#include <iostream>
#include <string>

#include "controller.h"
#include "event.h"
#include "report.h"

void PrintUsage(const char* program_name) {
  std::cerr << "Usage: " << program_name
            << " <num_trucks> <num_stations> [sim_minutes]\n"
            << "  <num_trucks>     Number of mining trucks (required)\n"
            << "  <num_stations>   Number of unload stations (required)\n"
            << "  [sim_minutes]    Duration of simulation in minutes "
               "(optional, default: 4320)\n";
}

int main(int argc, char** argv) {
  if (argc < 3) {
    PrintUsage(argv[0]);
    return EXIT_FAILURE;
  }

  size_t num_trucks = 0;
  size_t num_stations = 0;
  minutes_t sim_time = 72 * 60min;  // Default: 72 hours

  try {
    num_trucks = std::stoul(argv[1]);
    num_stations = std::stoul(argv[2]);
    if (argc >= 4) {
      sim_time = minutes_t(std::stoul(argv[3]));
    }
  } catch (const std::exception& e) {
    std::cerr << "Error: Invalid argument.\n";
    PrintUsage(argv[0]);
    return EXIT_FAILURE;
  }
  ClearEvents();

  std::cout << "Running simulation with " << num_trucks << " trucks and "
            << num_stations << " stations for " << sim_time.count()
            << " minutes...\n";

  Controller controller(num_trucks, num_stations);
  auto start_time = std::chrono::steady_clock::now();
  controller.Run(sim_time);
  auto end_time = std::chrono::steady_clock::now();
  auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                         end_time - start_time)
                         .count();

  std::cout << "\nSimulation completed in " << duration_ms << " ms\n";

  return EXIT_SUCCESS;
}
