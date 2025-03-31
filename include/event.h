#ifndef INCLUDE_EVENT_H_
#define INCLUDE_EVENT_H_

#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include "minutes.h"

// Types of events that can occur in the simulation// Stream output for logging
// and debugging
enum class EventType { Idle = 0, Travel, Mine, Queue, Unload };

std::string EventTypeToString(EventType type);

// Represents a simulation event involving a truck (and optionally a
// station)
struct Event {
  EventType type;
  size_t truck_id;
  std::optional<size_t> station_id;
  minutes_t start_time;
  minutes_t end_time;
};

// Stream output for logging and debugging
std::ostream& operator<<(std::ostream& os, const Event& event);

// Stores and provides access to all logged simulation events
class EventLogger {
 public:
  void LogEvent(const Event& event);

  const std::vector<Event>& GetEvents() const;
  std::vector<Event>& GetEvents();

 private:
  std::vector<Event> events_;
};

// Global event logger access
EventLogger& GetEventLogger();

// Convenience wrapper for logging events
void LogEvent(const Event& event);

#endif  // INCLUDE_EVENT_H_
