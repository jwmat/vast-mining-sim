#include "event.h"

#include <optional>
#include <string>
#include <vector>

#include "logger.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Converts an Event to a human-readable string
std::string Event::to_string() const {
  std::ostringstream os;
  os << *this;
  return os.str();
}

// Allows events to be ordered by start_time (for priority queues, sorting,
// etc.)
bool operator<(const Event& lhs, const Event& rhs) {
  return lhs.start_time < rhs.start_time;
}

// Outputs a formatted string representing the event's key attributes
std::ostream& operator<<(std::ostream& os, const Event& event) {
  os << "[" << EventTypeToString(event.type) << "] "
     << "[Truck " << event.truck_id << "]";
  if (event.station_id.has_value()) {
    os << " [Station " << event.station_id.value() << "]";
  }
  os << " Start: " << event.start_time << " End: " << event.end_time;
  return os;
}

// Maps EventType enum to string for serialization/logging
std::string EventTypeToString(EventType type) {
  switch (type) {
    case EventType::Mine:
      return "Mine";
    case EventType::TravelToStation:
      return "TravelToStation";
    case EventType::TravelToMine:
      return "TravelToMine";
    case EventType::Unload:
      return "Unload";
    case EventType::Queue:
      return "Queue";
  }
  return "";
}

// Parses a string back into an EventType enum
EventType EventTypeFromString(const std::string& s) {
  if (s == "Mine") return EventType::Mine;
  if (s == "TravelToStation") return EventType::TravelToStation;
  if (s == "TravelToMine") return EventType::TravelToMine;
  if (s == "Unload") return EventType::Unload;
  if (s == "Queue") return EventType::Queue;

  Logger::LogError("Unknown event type string: " + s);
  throw std::runtime_error("Unknown event type: " + s);
}

// Serializes an Event to JSON format
json EventToJson(const Event& event) {
  json j;
  j["type"] = EventTypeToString(event.type);
  j["truck_id"] = event.truck_id;
  j["station_id"] =
      event.station_id.has_value() ? json(event.station_id.value()) : nullptr;
  j["start_time"] = event.start_time.count();
  j["end_time"] = event.end_time.count();
  return j;
}

// Parses JSON back into an Event structure
Event JsonToEvent(const json& j) {
  Event event;
  event.type = EventTypeFromString(j.at("type").get<std::string>());
  event.truck_id = j.at("truck_id").get<size_t>();

  if (j.contains("station_id") && !j.at("station_id").is_null())
    event.station_id = j.at("station_id").get<size_t>();
  else
    event.station_id.reset();

  event.start_time = minutes_t(j.at("start_time").get<size_t>());
  event.end_time = minutes_t(j.at("end_time").get<size_t>());
  return event;
}

// Initializes the EventLogger with a flush thread and opens output file
EventLogger::EventLogger(const std::string& filename) : filename_(filename) {
  ofs_.open(filename, std::ios::app);
  if (!ofs_.is_open()) {
    Logger::LogError("Unable to open log file for writing: " + filename);
    throw std::runtime_error("Unable to open log file for writing: " +
                             filename);
  }
  flush_thread_ = std::thread([this] {
    while (!done_.load()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      FlushBuffer();  // Periodically flush buffered events to disk
    }
    FlushBuffer();  // Final flush on shutdown
  });
}

// Gracefully stops the background flush thread and closes files
EventLogger::~EventLogger() {
  done_ = true;
  if (flush_thread_.joinable()) flush_thread_.join();
  CloseStreams();
}

// Adds an event to the buffer, logs trace output
void EventLogger::LogEvent(const Event& event) {
  std::lock_guard<std::mutex> lock(buffer_mutex_);
  buffer_.push_back(event);
  Logger::LogTrace(event.to_string());
}

// Writes buffered events to disk and clears the buffer
void EventLogger::FlushBuffer() {
  std::lock_guard<std::mutex> lock(buffer_mutex_);
  for (const auto& e : buffer_) {
    ofs_ << EventToJson(e).dump() << "\n";
  }
  buffer_.clear();
  ofs_.flush();
}

// Waits for the background flush thread to finish flushing
void EventLogger::WaitUntilFlushed() {
  FlushBuffer();
  while (true) {
    {
      std::lock_guard<std::mutex> lock(buffer_mutex_);
      if (buffer_.empty()) break;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

// Reads the next event from file, skipping blank lines
bool EventLogger::ReadNextEvent(Event* event) {
  if (!ifs_.is_open()) {
    ifs_.open(filename_);
    if (!ifs_.is_open()) {
      Logger::LogError("Unable to open log file for reading: " + filename_);
      throw std::runtime_error("Unable to open log file for reading: " +
                               filename_);
    }
  }

  std::string line;
  if (std::getline(ifs_, line)) {
    if (line.empty()) return ReadNextEvent(event);  // recurse to skip blanks
    json j = json::parse(line);
    *event = JsonToEvent(j);
    return true;
  }
  return false;
}

// Truncates the log file, removing all prior events
void EventLogger::ClearEvents() {
  CloseStreams();
  ofs_.open(filename_, std::ios::out | std::ios::trunc);
  if (!ofs_.is_open()) {
    Logger::LogError("Unable to open log file for writing: " + filename_);
    throw std::runtime_error("Unable to open log file for writing: " +
                             filename_);
  }
}

// Flushes and closes both input/output streams
void EventLogger::CloseStreams() {
  FlushBuffer();
  if (ofs_.is_open()) {
    ofs_.flush();
    ofs_.close();
  }
  if (ifs_.is_open()) {
    ifs_.close();
  }
}

// Global shared EventLogger instance (singleton-like)
namespace {
std::shared_ptr<EventLogger> logger =
    std::make_shared<EventLogger>("events.json");
}

// Convenience global functions that proxy to the singleton
EventLogger& GetEventLogger() { return *logger; }
void LogEvent(const Event& event) { GetEventLogger().LogEvent(event); }
bool ReadEvent(Event* event) { return GetEventLogger().ReadNextEvent(event); }
void ClearEvents() { GetEventLogger().ClearEvents(); }
void WaitUntilFlushed() { GetEventLogger().WaitUntilFlushed(); }
