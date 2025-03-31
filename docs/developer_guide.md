# Developer Guide

This document is intended for contributors and maintainers of the Vast Mining Simulator.

---

## Project Structure

```
project-root/
├── build/                    # CMake build directory (not checked in)
│   └── events.<params>.json  # Events output file
│   └── metrics.<params>.json # Metrics output file
├── include/                  # Public headers
│   ├── controller.h
│   ├── event.h
│   ├── logger.h
│   ├── report.h
│   ├── station_manager.h
│   ├── truck_manager.h
│   └── ...
├── source/                 # Source files
│   ├── controller.cpp
│   ├── event.cpp
│   ├── logger.cpp
│   ├── report.cpp
│   └── ...
├── test/                   # Unit and integration tests
│   ├── test-controller.cpp
│   ├── test-logger.cpp
│   └── ...
├── scripts/                # Python visualization and utilities
│   └── plot_report.py
├── environment.yml         # Conda environment definition
├── CMakeLists.txt          # Top-level CMake file
├── .pre-commit-config.yaml # Pre-commit hooks configuration
└── README.md
```

---

## Components

- **Controller**: Orchestrates the entire simulation
- **TruckManager**: Maintains availability of trucks
- **StationManager**: Tracks station occupancy and queuing
- **MineManager**: Provides mining durations and logs mining events
- **Logger / EventLogger**: Captures event data and logs activity to the console and an optional file
- **Report**: Computes truck and station metrics post-simulation

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

- Tests are located in the `test/` directory and use GoogleTest
- Use `EXPECT_*` or `ASSERT_*` macros to validate behavior

Example:
```cpp
TEST(TestTruckManager, DispatchOrder) {
  TruckManager manager(3);
  auto [time1, id1] = manager.DispatchTruck();
  EXPECT_EQ(id1, 0);
}
```

---

## Debugging

- You can use the console output, optional log file, and `events.<params>.json` to trace behavior step-by-step.

---

## Formatting and Style

- C++ code is formatted using `clang-format` (configured via `.clang-format`)
- A `.pre-commit-config.yaml` is available for automated formatting of non-C++ files
- A `cpplint` target is available to check code style against Google's C++ guidelines

Run formatting from build directory:
```bash
cmake --build . --target format
```

Run cpplint from build directory:
```bash
cmake --build . --target cpplint
```

Run pre-commit:
```bash
pre-commit run --all-files
```
