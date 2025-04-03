# Developer Guide

This document is intended for contributors and maintainers of the Vast Mining Simulator.

---

## Project Structure

```
project-root/
├── build/                      # CMake build directory (not checked in)
│   └── events.json             # Events output file
│   └── metrics.<params>.json  # Metrics output file
├── include/                    # Public headers
│   ├── controller.h
│   ├── event.h
│   ├── logger.h
│   ├── report.h
│   └── ...
├── source/                     # Source files
│   ├── controller.cpp
│   ├── event.cpp
│   ├── logger.cpp
│   ├── report.cpp
│   └── ...
├── test/                       # Unit and integration tests
│   ├── test-controller.cpp
│   ├── test-logger.cpp
│   └── ...
├── scripts/                    # Python visualization and utilities
│   └── plot_report.py
├── environment.yml             # Conda environment definition
├── CMakeLists.txt              # Top-level CMake file
├── .pre-commit-config.yaml     # Pre-commit hooks configuration
└── README.md
```

---

## Components

- **Controller**: Orchestrates the simulation, manages trucks, station scheduling, and event lifecycle
- **StationQueue**: Manages availability and scheduling of unload stations
- **EventLogger**: Records all simulation events for traceability and debugging
- **Report**: Calculates per-truck and per-station metrics and exports results
- **Logger**: Configures spdlog-based asynchronous logging system

---

## Development Setup

1. Clone the repository
2. Create and activate the environment:

```bash
conda env create -f environment.yml
conda activate vast-mining-sim
```

3. Configure and build:

```bash
cmake -S . -B build
cmake --build build --config Debug
```

4. Run the simulation or tests:

```bash
./build/simulator_main 30 5 1440
cd build && ctest --output-on-failure
```

---

## Adding Tests

- Tests are located in the `test/` directory using GoogleTest
- Use `EXPECT_*` or `ASSERT_*` macros to validate correctness
- Coverage includes event ordering, metrics generation, and safety checks

Example:

```cpp
// Verifies that trucks unload in the same order they finished mining
TEST_P(TestController_WithParams, UnloadOrderMatchesMiningOrder) {
  Controller controller(10, 1);
  controller.Run(72 * 60min);

  MinHeap mining_order;
  MinHeap unloading_order;

  Event event;
  while (ReadEvent(&event)) {
    if (event.type == EventType::Mine)
      mining_order.push({event.end_time, event});
    if (event.type == EventType::Unload)
      unloading_order.push({event.start_time, event});
  }

  while (!mining_order.empty() && !unloading_order.empty()) {
    ASSERT_EQ(mining_order.top().second.truck_id,
              unloading_order.top().second.truck_id);
    mining_order.pop();
    unloading_order.pop();
  }
}
```

---

## Debugging

- Console and file logs are available from the `Logger` subsystem
- All events are logged to `events.json` in JSON Lines format
- Metrics are written to a file: `metrics.<params>.json`
- Use `scripts/plot_report.py` to visualize truck and station efficiency

---

## Formatting and Style

- C++ code is formatted using `clang-format` (configured via `.clang-format`)
- A `.pre-commit-config.yaml` handles auto-formatting for Python and other files
- C++ style guide is enforced using `cpplint`
- Run checks and formatting using:

Format C++:
```bash
cmake --build . --target format
```

Run linter:
```bash
cmake --build . --target cpplint
```

Run all pre-commit hooks:
```bash
pre-commit run --all-files
```
