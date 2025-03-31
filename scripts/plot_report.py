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


def load_json(path):
    """Load JSON data from a file."""
    with open(path, "r") as f:
        return json.load(f)


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
        elif event_type == "Travel":
            truck_stats[truck_id]["travel"] += duration
        elif event_type == "Queue":
            truck_stats[truck_id]["queue"] += duration
        elif event_type == "Unload":
            truck_stats[truck_id]["unload"] += duration
            truck_stats[truck_id]["trips_completed"] += 1

        # Station stats
        if event_type == "Unload" and "station_id" in e:
            station_id = e["station_id"]
            station_stats[station_id]["unload"] += duration
            station_stats[station_id]["unload_count"] += 1

    # Calculate truck efficiency (mining time / simulation duration)
    for stats in truck_stats.values():
        efficiency = (stats["mine"] + stats["travel"] + stats["unload"]) / (
            stats["mine"] + stats["travel"] + stats["unload"] + stats["queue"]
        )
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
    """Plot truck performance metrics like total mines completed."""
    total_mines = [stats["trips_completed"] for stats in truck_stats.values()]

    ax.bar(range(len(total_mines)), total_mines, color=COLOR_SCHEME["mining"], edgecolor="black")
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
    parser.add_argument("--events", default="events.json", help="Path to events.json")
    args = parser.parse_args()

    # Load events from the JSON file
    data = load_json(args.events)

    # Handle both dict and list formats
    if isinstance(data, dict) and "events" in data:
        simulation_duration = data["simulation_duration"]
        events = data["events"]
    elif isinstance(data, list):
        simulation_duration = 72 * 60  # Default duration
        events = data
    else:
        raise ValueError("Invalid JSON format: expected a list or a dictionary with an 'events' key.")

    # Process events and gather stats
    truck_stats, station_stats, truck_efficiency, station_efficiency = process_events(events, simulation_duration)

    # Create a 2x2 subplot layout
    fig, axs = plt.subplots(2, 2, figsize=(14, 12))

    # Plot truck efficiency histogram
    plot_truck_efficiency_histogram(truck_efficiency, axs[0, 0])

    # Plot station efficiency histogram
    plot_station_efficiency_histogram(station_efficiency, axs[0, 1])

    # Plot truck performance (total mines completed)
    plot_truck_performance(truck_stats, axs[1, 0])

    # Plot station performance (total unloads)
    plot_station_performance(station_stats, axs[1, 1])

    # Show all plots at once
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()
