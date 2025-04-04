import json
import matplotlib.pyplot as plt
import numpy as np
from collections import defaultdict
import argparse

# Define consistent color scheme for all plots
COLOR_SCHEME = {
    "mining": "#1F5673",
    "unload": "#D79E42",
}


def load_events(path):
    """Load events from a JSON Lines file."""
    events = []
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            if line:  # skip empty lines
                try:
                    event = json.loads(line)
                    events.append(event)
                except json.JSONDecodeError as ex:
                    print("Skipping invalid JSON line:", line)
    return events


def process_events(events, simulation_duration):
    """Process events to gather truck stats, station stats, and calculate efficiency."""
    truck_stats = defaultdict(lambda: {"mine": 0, "travel": 0, "queue": 0, "unload": 0, "trips_completed": 0})
    station_stats = defaultdict(lambda: {"unload": 0, "unload_count": 0})
    truck_efficiency = []
    station_efficiency = []

    # Process each event
    for e in events:
        truck_id = e["truck_id"]
        start_time = e["start_time"]
        end_time = e["end_time"]
        duration = end_time - start_time
        event_type = e["type"]

        # Truck stats
        if event_type == "Mine":
            truck_stats[truck_id]["mine"] += duration
        elif event_type == "TravelToStation" or event_type == "TravelToMine" or event_type == "Travel":
            truck_stats[truck_id]["travel"] += duration
        elif event_type == "Queue":
            truck_stats[truck_id]["queue"] += duration
        elif event_type == "Unload":
            truck_stats[truck_id]["unload"] += duration
            truck_stats[truck_id]["trips_completed"] += 1

        # Station stats
        if event_type == "Unload" and "station_id" in e and e["station_id"] is not None:
            station_id = e["station_id"]
            station_stats[station_id]["unload"] += duration
            station_stats[station_id]["unload_count"] += 1

    # Calculate truck efficiency (active time / total time)
    for stats in truck_stats.values():
        efficiency = (stats["mine"] + stats["travel"] + stats["unload"]) / simulation_duration
        truck_efficiency.append(efficiency)

    # Calculate station efficiency (unloading time / simulation duration)
    for stats in station_stats.values():
        efficiency = stats["unload"] / simulation_duration
        station_efficiency.append(efficiency)

    return truck_stats, station_stats, truck_efficiency, station_efficiency


def plot_truck_efficiency_histogram(truck_efficiency, ax):
    """Plot truck efficiency histogram."""
    ax.hist(truck_efficiency, bins=20, edgecolor="black", color=COLOR_SCHEME["mining"])
    ax.set_title("Truck Efficiency")
    ax.set_xlabel("Efficiency (Active Time / Simulation Duration)")
    ax.set_ylabel("Number of Trucks")
    ax.grid(True, axis="y", linestyle="--", alpha=0.4)


def plot_station_efficiency_histogram(station_efficiency, ax):
    """Plot station efficiency histogram."""
    ax.hist(station_efficiency, bins=20, edgecolor="black", color=COLOR_SCHEME["unload"])
    ax.set_title("Station Efficiency")
    ax.set_xlabel("Efficiency (Unloading Time / Simulation Duration)")
    ax.set_ylabel("Number of Stations")
    ax.grid(True, axis="y", linestyle="--", alpha=0.4)


def plot_truck_performance(truck_stats, ax):
    """Plot truck performance metrics like total trips completed."""
    total_trips = [stats["trips_completed"] for stats in truck_stats.values()]
    ax.bar(range(len(total_trips)), total_trips, color=COLOR_SCHEME["mining"], edgecolor="black")
    ax.set_title("Truck Performance: Total Trips Completed")
    ax.set_xlabel("Truck ID")
    ax.set_ylabel("Total Trips")
    ax.grid(True, axis="y", linestyle="--", alpha=0.4)


def plot_station_performance(station_stats, ax):
    """Plot station performance metrics like total unloads."""
    total_unloads = [stats["unload_count"] for stats in station_stats.values()]
    ax.bar(range(len(total_unloads)), total_unloads, color=COLOR_SCHEME["unload"], edgecolor="black")
    ax.set_title("Station Performance: Total Unloads")
    ax.set_xlabel("Station ID")
    ax.set_ylabel("Total Unloads")
    ax.grid(True, axis="y", linestyle="--", alpha=0.4)


def main():
    """Main function to load data, process events, and plot results."""
    parser = argparse.ArgumentParser()
    parser.add_argument("--events", help="Path to events.jsonl", required=True)
    args = parser.parse_args()

    # Load events from the JSON Lines file
    events = load_events(args.events)
    if not events:
        raise ValueError("No events loaded from file.")

    # Determine simulation_duration as the maximum end_time among all events
    simulation_duration = max(e["end_time"] for e in events)

    # Process events and gather stats
    truck_stats, station_stats, truck_efficiency, station_efficiency = process_events(events, simulation_duration)

    # Create a 2x2 subplot layout
    fig, axs = plt.subplots(2, 2, figsize=(14, 12))

    # Plot truck efficiency histogram
    plot_truck_efficiency_histogram(truck_efficiency, axs[0, 0])

    # Plot station efficiency histogram
    plot_station_efficiency_histogram(station_efficiency, axs[0, 1])

    # Plot truck performance (total trips completed)
    plot_truck_performance(truck_stats, axs[1, 0])

    # Plot station performance (total unloads)
    plot_station_performance(station_stats, axs[1, 1])

    # Show all plots at once
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()
