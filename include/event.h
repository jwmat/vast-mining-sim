#ifndef INCLUDE_EVENT_H_
#define INCLUDE_EVENT_H_

#include <memory>
#include <optional>
#include <ostream>
#include <vector>

#include "minutes.h"

enum class EventType { Idle = 0, Travel, Mine, Queue, Unload };

struct Event {
  EventType type;
  size_t truck_id;
  std::optional<size_t> station_id;
  minutes_t start_time;
  minutes_t end_time;
  std::optional<minutes_t> wait_time;
};

std::ostream& operator<<(std::ostream& os, const Event& event);

class EventLogger {
 public:
  void LogEvent(const Event& event);

  const std::vector<Event>& GetEvents() const;

 private:
  std::vector<Event> events_;
};

EventLogger& GetEventLogger();

void LogEvent(const Event& event);

#endif  // INCLUDE_EVENT_H_
