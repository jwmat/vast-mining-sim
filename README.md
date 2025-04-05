# Vast Mining Simulator

This project simulates a lunar Helium-3 mining operation, modeling the behavior of mining trucks and unload stations over a configurable 72-hour window.

The simulation is designed to be accurate, extensible, and testable, with a focus on performance tracking, reproducibility, and clean architecture.

---

## Project Features

- Configurable number of mining trucks and unload stations
- Randomized mining durations (1–5 hours)
- 30-minute travel time and 5-minute unload time per truck
- Queuing logic based on station availability
- Efficiency and utilization tracking for all trucks and stations
- Simulation duration configurable (default: 72 hours)
- JSON output of events and metrics for further visualization or analysis
- Detailed unit tests with GoogleTest
- Reproducible random simulation behavior via compile-time seed

---

## Build Instructions

### Option 1: Recommended (Using Conda)

This project includes a `conda` environment for dependency management:

```bash
conda env create -f environment.yml
conda activate vast-mining-sim
cmake -S . -B build
cmake --build build --config Debug
```

> Includes GoogleTest, nlohmann_json, and other helpful tools.

### Option 2: Without Conda

If you're not using Conda, CMake will automatically fetch dependencies like GoogleTest and nlohmann/json via `FetchContent`:

```bash
cmake -S . -B build
cmake --build build --config Debug
```

---

## Running Tests

After building:

```bash
cd build
ctest --output-on-failure
```

Or run the individual test binaries from `build/bin`.

---

## Simulation Parameters

You can configure:

- Number of trucks and stations
- Simulation duration (in minutes)

---


## Output & Reporting

Simulation results include:

- A full timeline of simulation events, useful for playback or visual debugging: `events.<params>.json`
- A detailed breakdown of per-truck and per-station performance metrics (generated after each run): `metrics.<params>.json`
- Console summary:
  - Simulation parameters (e.g., trucks, stations, duration)
  - Average truck and station utilization (%)
  - Path to the saved metrics file

You can visualize the simulation results using the included Python script:

```bash
python ../scripts/plot_report.py --events events.json
```
