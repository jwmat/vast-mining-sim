#include "logger.h"

#include <chrono>  // NOLINT(build/c++11)
#include <iostream>
#include <ostream>
#include <sstream>

// Default logger implementation: writes to std::cout
void Logger::Log(LogLevel level, const std::string& message) {
  std::string level_str = GetLogLevelString(level);
  std::string timestamp = GetTimestamp();
  std::ostringstream log_message;

  log_message << "[" << timestamp << "] [" << level_str << "] " << message;

  std::cout << log_message.str() << std::endl;
}

// Converts enum LogLevel to a readable string
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

// Returns a formatted timestamp: "YYYY-MM-DD HH:MM:SS"
std::string Logger::GetTimestamp() const {
  auto now = std::chrono::system_clock::now();
  auto time_t_now = std::chrono::system_clock::to_time_t(now);
  std::tm* local_time = std::localtime(&time_t_now);

  std::ostringstream timestamp;
  timestamp << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");

  return timestamp.str();
}

// Returns a timestamp formatted for use in filenames
std::string FileLogger::GetFileNameTimestamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
  std::tm* local_time = std::localtime(&time_t_now);

  std::stringstream timestamp;
  timestamp << std::put_time(local_time, "%Y-%m-%d_%H-%M-%S");
  return timestamp.str();
}

// Opens the log file (uses timestamp-based name if none provided)
FileLogger::FileLogger(std::string filename) {
  if (filename.empty()) {
    filename = "log_" + GetFileNameTimestamp() + ".txt";
  }

  file_.open(filename, std::ios::app);
  if (!file_.is_open()) {
    throw std::runtime_error("Failed to open log file: " + filename);
  }
}

// Closes the log file on destruction
FileLogger::~FileLogger() {
  if (file_.is_open()) {
    file_.close();
  }
}

// Writes a log message to the file with timestamp and level
void FileLogger::Log(LogLevel level, const std::string& message) {
  std::string level_str = GetLogLevelString(level);
  std::string timestamp = GetTimestamp();
  std::ostringstream log_message;

  log_message << "[" << timestamp << "] [" << level_str << "] " << message;
  file_ << log_message.str() << std::endl;
}

// Global logger instance (initialized with FileLogger)
namespace {
std::shared_ptr<Logger> logger = std::make_shared<FileLogger>();
}

// Returns reference to the current global logger
Logger& GetLogger() { return *logger; }

// Replaces the global logger with a new one
void SetLogger(std::shared_ptr<Logger> new_logger) {
  logger = std::move(new_logger);
}

// Shorthand functions for logging at each level
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
