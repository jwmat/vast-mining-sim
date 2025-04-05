# Architecture Overview

This document provides overview of the architecture and component responsibilities of the simulator.

---

## Overview

The simulator models a lunar Helium-3 mining operation with trucks, mining sites, and unloading stations. The simulation is orchestrated by a central `Controller` and operates in a loop over simulated time
---

## Core Components

### Controller
- Acts as the main entry point for the simulation
- Manages the lifecycle of mining trucks and coordinates transitions between mining, traveling, and unloading
- Enforces the simulation time window and ensures no operations exceed the configured duration
- Schedules all events via a priority queue (`event_queue_`) ordered by timestamp
- Owns and tracks all metrics for trucks and stations

### StationQueue
- Wrapper around a min-heap that tracks station availability by timestamp
- Provides clean `PopNextAvailable()` and `MarkAvailable()` interfaces
- Handles all scheduling of unloading events and queue tracking

### Random Mining Duration
- Provides randomized mining durations between 60 and 300 minutes
- Internally uses `std::default_random_engine` seeded with a fixed value for reproducibility

### EventLogger
- Centralized event recording system.
- Captures all truck activities: mining, traveling, queuing, and unloading
- Events include start and end times, truck id, and optionally station id
- Supports retrieval for analysis or metrics generation

### Metrics / Report Generator
- Aggregates event data to compute per-truck and per-station performance metrics
- Outputs include:
  - Utilization percentage
  - Idle and queueing times
  - Number of completed trips and unloads
  - Average mining and queue durations
- Exports metrics and raw event logs to JSON for external use

### Python Visualizer
- Consumes the JSON event log
- Produces visual plots including:
  - Truck and Station Efficiency
  - Utilization breakdowns
- Used to quickly visualize efficiency and performanc

---

## Simulation Flow

1. Initialize station availability and simulation clock
2. Dispatch all trucks to begin mining
3. Trucks complete mining, travel to stations, unload, return to mine, and repeat
4. If an event would exceed the simulation time limit, it is skipped
5. Simulation ends when no more events can be scheduled within the time window

---

## Design Goals

- **Modularity**: Encapsulated responsibilities (Controller, StationQueue, Logger, Metrics)
- **Performance**: Priority queues ensure O(log N) operations
- **Reproducibility**: Controlled random seed ensures deterministic behavior
- **Traceability**: All events are persistently logged for audit and debugging
- **Extensibility**: Easy to add more detailed behaviors (e.g., mine assignment, station prioritization)

---

## Time & Space Complexity

-The simulation uses a discrete-event loop with efficient scheduling via min-heaps. The time complexity is driven by the number of mining-unloading cycles completed during the simulation.

Let:
- `N` = number of trucks
- `M` = number of stations
- `C` = number of completed truck cycles

### Time Complexity of `Controller::Run(sim_time)`

Each full truck cycle (mine → travel → queue → unload → travel → return) involves the following operations:

| Operation                    | Complexity     | Notes                                     |
|------------------------------|----------------|-------------------------------------------|
| Mining + Travel Scheduling   | O(1)           | Constant time, includes RNG               |
| Event Queue Push             | O(log C)       | Cumulative due to priority queue          |
| Station selection            | O(1)           | Min-heap top peek                         |
| Requeue station              | O(log M)       | After unloading                           |

**Total per cycle: O(log M + log C)**

**Overall runtime: O(C × (log M + log C))**

---

### Space Complexity

| Component         | Space Used  | Notes                                        |
|-------------------|-------------|----------------------------------------------|
| Event queue       | O(C)        | One entry per scheduled event                |
| StationQueue      | O(M)        | One entry per station                        |
| Truck metrics     | O(N)        | Fixed size per truck                         |
| Station metrics   | O(M)        | Fixed size per station                       |
| Event log         | O(C)        | One JSON line per recorded event             |

---

### Scalability

- Efficient for  hundreds to thousands of trucks/stations
- Runtime scales with event count, not clock time
- Async logging and efficient serialization ensure fast I/O
- Fully deterministic when seeded with a known value
