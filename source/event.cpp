#include "event.h"

#include <sstream>

#include "logger.h"
std::ostream& operator<<(std::ostream& os, const Event& event) {
  auto to_string = [](EventType type) -> std::string {
    switch (type) {
      case EventType::Travel:
        return "Travel";
      case EventType::Mine:
        return "Mine";
      case EventType::Queue:
        return "Queue";
      case EventType::Unload:
        return "Unload";
      default:
        return "Unknown";
    }
  };

  os << "[" << to_string(event.type) << "] "
     << "[Truck " << event.truck_id << "]";

  if (event.station_id.has_value()) {
    os << " [Station " << event.station_id.value() << "]";
  }

  os << " Start: " << event.start_time << " End: " << event.end_time;

  if (event.type == EventType::Queue && event.wait_time.has_value()) {
    os << " Wait: " << event.wait_time.value() << " min";
  }

  return os;
}

void EventLogger::LogEvent(const Event& event) {
  events_.push_back(event);
  std::ostringstream message;
  message << event;
  LogInfo(message.str());
}

const std::vector<Event>& EventLogger::GetEvents() const { return events_; }

namespace {
std::shared_ptr<EventLogger> logger = std::make_shared<EventLogger>();
}

EventLogger& GetEventLogger() { return *logger; }

void LogEvent(const Event& event) { GetEventLogger().LogEvent(event); }
