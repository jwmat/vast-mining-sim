#include "logger.h"

#include <iostream>
#include <ostream>
#include <sstream>

void Logger::Log(LogLevel level, const std::string& message) {
  std::string level_str = GetLogLevelString(level);
  std::string timestamp = GetTimestamp();
  std::ostringstream log_message;

  log_message << "[" << timestamp << "] [" << level_str << "] " << message;

  std::cout << log_message.str() << std::endl;
}

std::string Logger::GetLogLevelString(LogLevel level) const {
  switch (level) {
    case LogLevel::INFO:
      return "INFO";
    case LogLevel::DEBUG:
      return "DEBUG";
    case LogLevel::WARNING:
      return "WARNING";
    case LogLevel::ERROR:
      return "ERROR";
    default:
      return "UNKNOWN";  // Handles unexpected enum values
  }
}

std::string Logger::GetTimestamp() const {
  auto now = std::chrono::system_clock::now();
  auto time_t_now = std::chrono::system_clock::to_time_t(now);
  std::tm* local_time = std::localtime(&time_t_now);

  std::ostringstream timestamp;
  timestamp << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");

  return timestamp.str();
}

FileLogger::FileLogger(std::string filename) {
  if (filename.empty()) {
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_now), "%Y-%m-%d_%H-%M-%S");
    filename = "log_" + ss.str() + ".txt";
  }
  file_.open(filename, std::ios::app);
}

FileLogger::~FileLogger() {
  if (file_.is_open()) {
    file_.close();
  }
}

void FileLogger::Log(LogLevel level, const std::string& message) {
  std::string level_str = GetLogLevelString(level);
  std::string timestamp = GetTimestamp();
  std::ostringstream log_message;

  log_message << "[" << timestamp << "] [" << level_str << "] " << message;

  file_ << log_message.str() << std::endl;
}

namespace {
std::shared_ptr<Logger> logger = std::make_shared<Logger>();
}

Logger& GetLogger() { return *logger; }

void SetLogger(std::shared_ptr<Logger> new_logger) {
  logger = std::move(new_logger);
}

void LogInfo(const std::string& message) {
  GetLogger().Log(LogLevel::INFO, message);
}

void LogDebug(const std::string& message) {
  GetLogger().Log(LogLevel::DEBUG, message);
}

void LogWarning(const std::string& message) {
  GetLogger().Log(LogLevel::WARNING, message);
}

// Convenience helper function for logging error messages
void LogError(const std::string& message) {
  GetLogger().Log(LogLevel::ERROR, message);
}
