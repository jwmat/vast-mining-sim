# Sample Simulation Results & Analysis Report

This document presents a sample run of the Vast Mining Simulator, showcasing simulation output, efficiency metrics, and visual analysis.

---

## Configuration

**Simulation Parameters:**
- Trucks: 100
- Stations: 10
- Duration: 72 hours (4320 minutes)

The simulation was run using the following command:
```bash
./main 100 10 4320
```

---

## Metrics Summary

For each truck and station, the following metrics were computed:

Console output:

```
Running simulation with 100 trucks and 10 stations for 4320 minutes...

=== Simulation Summary ===
Simulation Time: 4320 minutes
Trucks: 100
Stations: 10
Average Truck Utilization: 98.10%
Average Station Utilization: 19.95%

Simulation completed in 127 ms
```

`metrics.<params>.json`:

```
{
  "simulation_duration": 4320,
  "stations": [
    {
      "avg_queueing_time": 0.0,
      "id": 0,
      "idle_time": 3445,
      "queueing_time": 0,
      "queues_completed": 0,
      "throughput": 175,
      "unloading_time": 875,
      "utilization": 20.25462962962963
    },
    ...
  ]
  "trucks": [
    {
      "avg_queueing_time": 0.0,
      "avg_trip_time": 262.1875,
      "id": 0,
      "idle_time": 125,
      "mines_completed": 16,
      "mining_time": 3155,
      "queueing_time": 0,
      "queues_completed": 0,
      "trips_completed": 16,
      "utilization": 97.10648148148148
    },
    ...
  ]
}
```

`events.json`:

```
{"end_time":212,"start_time":0,"station_id":null,"truck_id":0,"type":"Mine"}
{"end_time":170,"start_time":0,"station_id":null,"truck_id":1,"type":"Mine"}
{"end_time":241,"start_time":0,"station_id":null,"truck_id":2,"type":"Mine"}
...
```

---

## Visualization

The following plots were generated using the script:
```bash
python scripts/plot_report.py --events build/events.json
```

**Metrics Visualized:**
- Truck Efficiency (active time / sim time)
- Station Efficiency (unloading time / sim time)
- Truck Utilization
- Station Utilization

### Sample Output

![Simulation Metrics Visualization](images/efficiency_plot.png)

---
