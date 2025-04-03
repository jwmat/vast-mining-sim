#ifndef INCLUDE_EVENT_H_
#define INCLUDE_EVENT_H_

#include <fstream>
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "minutes.h"

// Defines the types of events that can occur in the simulation.
enum class EventType { TravelToStation, Mine, TravelToMine, Queue, Unload };

// Converts an EventType enum to a string for logging or serialization.
std::string EventTypeToString(EventType type);

// Parses a string into an EventType enum. Throws if unknown.
EventType EventTypeFromString(const std::string& s);

// Represents a discrete simulation event involving a truck (and optionally a
// station).
struct Event {
  EventType type;
  size_t truck_id;
  std::optional<size_t> station_id;
  minutes_t start_time;
  minutes_t end_time;

  std::string to_string() const;  // Human-readable summary
};

// Orders events by start time for use in priority queues.
bool operator<(const Event& lhs, const Event& rhs);

// Stream operator for readable debug output.
std::ostream& operator<<(std::ostream& os, const Event& event);

// Manages logging of simulation events to a file and reading them back.
class EventLogger {
 public:
  explicit EventLogger(const std::string& filename);
  ~EventLogger();

  // Appends a single event to the log (in JSON Lines format).
  void LogEvent(const Event& event);

  // Reads the next event from the log (returns false if no more).
  bool ReadNextEvent(Event* event);

  // Clears the log file by truncating it.
  void ClearEvents();

 private:
  std::string filename_;
  std::ofstream ofs_;
  std::ifstream ifs_;

  void CloseStreams();  // Internal cleanup
};

// Access the global logger instance.
EventLogger& GetEventLogger();

// Global logging utility functions.
void LogEvent(const Event& event);
bool ReadEvent(Event* event);
void ClearEvents();

#endif  // INCLUDE_EVENT_H_
