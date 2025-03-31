# Architecture Overview

This document provides overview of the architecture and component responsibilities of the simulator

---

## Overview

The simulator models a lunar Helium-3 mining operation with trucks, mining sites, and unloading stations. The simulation is orchestrated by a central `Controller` and operates in a loop over simulated time

---

## Core Components

### Controller
- Acts as the main entry point for the simulation
- Manages the lifecycle of mining trucks and coordinates transitions between mining, traveling, and unloading
- Enforces the simulation time window and ensures no operations exceed the configured duration

### TruckManager
- Maintains a min-heap of available trucks, sorted by their next availability time
- Handles truck dispatching and return to the queue after completing a cycle
- Ensures that only one operation per truck is scheduled at a time

### StationManager
- Uses a min-heap to track unloading station availability
- Handles unloading operations, including tracking queueing time if a station is occupied

### MineManager
- Provides randomized mining durations between 60 and 300 minutes
- Generates mining events and returns the expected end time
- Configurable via a compile-time random seed (`RANDOM_SEED`) to ensure reproducible simulations

### EventLogger
- Centralized event recording system.
- Captures all truck activities: mining, traveling, queuing, and unloading
- Events include start and end times, truck id, and optionally station id
- Supports retrieval for analysis or metrics generation

### Metrics / Report Generator
- Consumes the event log to produce per-truck and per-station performance metrics
- Outputs include:
  - Utilization percentage
  - Idle and queueing times
  - Number of completed trips and unloads
  - Average mining and queue durations
- Supports export of all events and per truck/station metrics to a JSON file for external use

### Python Visualizer
- Consumes the JSON event log
- Produces four plots:
  - Truck Efficiency (active time / sim time)
  - Station Efficiency (unloading time / sim time)
  - Truck Utilization
  - Station Utilization
- Used to quickly visualize efficiency and performance

---

## Simulation Flow

1. Initialize trucks and stations with start availability at time zero
2. Dispatch all trucks to mine
3. Trucks complete mining, travel to a station, unload, return to mine, and repeat
4. If any step exceeds the simulation time, that truck exits the cycle
5. Continue looping until no trucks are eligible for further dispatch

---

## Design Goals

- **Modularity**: Components like `TruckManager`, `StationManager`, and `MineManager` are isolated for testability and reuse
- **Performance**: Simulation runs faster than real-time; uses priority queues to efficiently track availability
- **Reproducibility**: Random behavior is deterministic based on compile-time seed
- **Traceability**: All actions are logged and reportable
- **Extensibility**: Easy to add more detailed behaviors (e.g., mine assignment, station prioritization)

---

## Time & Space Complexity

The simulation uses a discrete-event loop with efficient scheduling via min-heaps. The time complexity is driven by the number of mining-unloading cycles completed during the simulation.

Let:
- `N` = number of trucks
- `M` = number of unload stations
- `C` = total number of full mining cycles executed across all trucks

---

### Time Complexity of `Controller::Run(sim_time)`

Each full truck cycle (mine → travel → queue → unload → travel → return) involves the following operations:

| Operation                     | Complexity     | Reasoning                                          |
|-------------------------------|----------------|----------------------------------------------------|
| Dispatch truck                | O(log N)       | Pop from truck availability min-heap               |
| Choose station (peek)         | O(1)           | Peek top of station availability heap              |
| Requeue station               | O(log M)       | Push back into station heap after unloading        |
| Return truck                  | O(log N)       | Push back into truck heap after mining             |
| Generate mining duration      | O(1)           | Uniform random number                              |
| Log each event                | O(1)           | Push to event vector                               |

Each cycle: **O(log N + log M)**

Total over all cycles (C):
**O(C × (log N + log M))**

This is the dominant cost of the simulation.

---

### Space Complexity

| Component         | Space Used        | Notes                                      |
|-------------------|-------------------|--------------------------------------------|
| Event log         | O(C)              | One entry per event (travel, mine, unload) |
| Truck heap        | O(N)              | At most one entry per truck                |
| Station heap      | O(M)              | At most one entry per station              |
| Truck metrics     | O(N)              | Aggregated after simulation                |
| Station metrics   | O(M)              | Aggregated after simulation                |

---

### Scalability

- Efficient for hundreds to thousands of trucks and stations
- Simulation runtime scales with the number of active truck cycles, not with clock time
- Runs significantly faster than real-time on modern machines
- Fully deterministic when a seed is provided (`RANDOM_SEED`)
