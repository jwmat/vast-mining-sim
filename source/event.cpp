#include "event.h"

#include <sstream>

#include "logger.h"

std::string EventTypeToString(EventType type) {
  switch (type) {
    case EventType::Mine:
      return "Mine";
    case EventType::Travel:
      return "Travel";
    case EventType::Queue:
      return "Queue";
    case EventType::Unload:
      return "Unload";
    default:
      return "Unknown";
  }
}

// Formats an Event for output to a stream
std::ostream& operator<<(std::ostream& os, const Event& event) {
  auto event_str = EventTypeToString(event.type);
  os << "[" << event_str << "] "
     << "[Truck " << event.truck_id << "]";
  if (event.station_id.has_value()) {
    os << " [Station " << event.station_id.value() << "]";
  }
  os << " Start: " << event.start_time << " End: " << event.end_time;
  return os;
}

// Stores the event and logs it to the logger
void EventLogger::LogEvent(const Event& event) {
  events_.push_back(event);
  std::ostringstream message;
  message << event;
  LogInfo(message.str());
}

const std::vector<Event>& EventLogger::GetEvents() const { return events_; }
std::vector<Event>& EventLogger::GetEvents() { return events_; }

namespace {
std::shared_ptr<EventLogger> logger = std::make_shared<EventLogger>();
}

EventLogger& GetEventLogger() { return *logger; }

void LogEvent(const Event& event) { GetEventLogger().LogEvent(event); }
