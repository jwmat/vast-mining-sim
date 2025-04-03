# User Guide

This guide provides example commands and walkthroughs for running the simulator, viewing the output, and interpreting the results.

---

## Quickstart

After building the project (see README or Developer Guide), run the simulator using the main executable:

```bash
./main <num_trucks> <num_stations> [sim_minutes]
```

### Example

```bash
./main 30 5 1440
```

This runs a 24-hour simulation (1440 minutes) with 30 trucks and 5 unloading stations.

---

## Parameters

| Parameter      | Description                                 | Default       |
|----------------|---------------------------------------------|---------------|
| `num_trucks`   | Number of mining trucks                     | Required      |
| `num_stations` | Number of unload stations                   | Required      |
| `sim_minutes`  | Duration of the simulation (in minutes)     | 4320 (72 hrs) |

---

## Output Files

The simulator generates three primary outputs:

### 1. Console Summary

A brief performance summary printed to stdout:

```
=== Simulation Summary ===
Simulation Time: 4320 minutes
Trucks: 10
Stations: 1
Average Truck Utilization: 98.30%
Average Station Utilization: 19.91%

Full metrics report: metrics.10truck_1station_4320min_minutes.json

Simulation completed in 15 ms
```

A hint to the corresponding metrics file is printed at the bottom.

---

### 2. Metrics Report (JSON)

A structured report of per-truck and per-station metrics is saved as:

```
metrics.<num_trucks>truck_<num_stations>station_<sim_minutes>_minutes.json
```

Metrics include:
- Utilization and idle times
- Number of trips, mines, unloads
- Total and average times spent mining, traveling, and queueing

---

### 3. Event Log (JSON Lines Format)

Every event (mining, travel, unload, etc.) is logged chronologically in:

```
events.json
```

Each line is a JSON object:

```json
{"end_time":2211,"start_time":2206,"station_id":6,"truck_id":14,"type":"Unload"}
{"end_time":2426,"start_time":2206,"station_id":null,"truck_id":97,"type":"Mine"}
{"end_time":2236,"start_time":2206,"station_id":null,"truck_id":69,"type":"TravelToMine"}
{"end_time":2236,"start_time":2206,"station_id":null,"truck_id":64,"type":"TravelToMine"}
{"end_time":2213,"start_time":2208,"station_id":0,"truck_id":60,"type":"Unload"}
{"end_time":2213,"start_time":2208,"station_id":1,"truck_id":26,"type":"Unload"}
{"end_time":2239,"start_time":2209,"station_id":null,"truck_id":86,"type":"TravelToStation"}
{"end_time":2214,"start_time":2209,"station_id":7,"truck_id":30,"type":"Unload"}
```

This log is useful for event replay, debugging, or advanced analytics.

---

## Visualizing the Output

A Python visualization tool is provided to plot summary charts:

```bash
python scripts/plot_report.py --events events.json
```

This generates four charts:
- Truck Efficiency (active time / sim time)
- Station Efficiency (unloading time / sim time)
- Truck Utilization
- Station Utilization

Ensure your environment includes matplotlib and pandas (see `environment.yml`).

---

## Sample Run

Run a quick simulation with 30 trucks and 5 stations for 24 hours:

```bash
./main 30 5 1440
```

Expected runtime: ~15 ms on a modern CPU

Outputs:
- `metrics.30truck_5station_1440_minutes.json`
- `events.json`
- Console summary

---

## Notes

- All output is deterministic with a fixed seed
- Run `ctest` from the `build/` directory to verify system behavior
- You can configure logging verbosity or output filenames in `logger.cpp`

For advanced usage, see the Developer Guide.
