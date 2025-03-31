# User Guide

This guide provides example commands and walkthrough for running the simulator, viewing the output, and interpreting the results.

---

## Quickstart

After building the project (see README), run the simulator using one of the test or main entry points.

The main executable is run as follows:

```bash
./main <num_trucks> <num_stations> [sim_minutes]
```

### Example
```bash
./main 30 5 1440
```

This runs a 24-hour simulation with 30 trucks and 5 stations.

---

## Parameters

| Parameter        | Description                                     | Default       |
|------------------|-------------------------------------------------|---------------|
| `num_trucks`     | Number of mining trucks                         | Required      |
| `num_stations`   | Number of unload stations                       | Required      |
| `sim_minutes`    | Duration of the simulation (in minutes)         | 4320 (72 hrs) |

---

## Expected Output

The simulation produces three types of output:

### 1. Console Metrics
Summarized stats for trucks and stations:

```
=== Simulation Summary ===
Simulation Time: 4320 minutes
Trucks: 1000
Stations: 1
Average Truck Utilization: 8.77%
Average Station Utilization: 97.92%

Detailed metrics for individual trucks and stations can be found in metrics.1000truck_1station_4320min.json
```

### 2. Logged Metrics

A structured JSON file containing detailed per-truck and per-station metrics such as:

- Total trips/throughput
- Average queueing and/or mining durations
- Utilization and idle times

This file is ideal for external analysis or batch processing. It is generated automatically at the end of each simulation and saved in the build directory as `metrics.<params>.json`.


### 3. JSON Event Log
A structured log of all simulation events saved to `events.<params>.json`:

```json
{
  "simulation_duration": 4320,
  "events": [
    {
      "type": "Mine",
      "truck_id": 0,
      "start_time": 0,
      "end_time": 180
    },
    ...
  ]
}
```

---

## Visualizing the Output

You can plot utilization and efficiency charts with the Python visualizer:

```bash
python ../scripts/plot_report.py --events events.<params>.json
```

This generates four plots:
- Truck Efficiency (active time / sim time)
- Station Efficiency (unloading time / sim time)
- Truck Utilization
- Station Utilization

---

## Sample Run

Run a quick simulation with 30 trucks and 5 stations for 24 hours:

```bash
./main 30 5 1440
```

Expected runtime: 9 ms

Result: JSON files and printed performance stats
