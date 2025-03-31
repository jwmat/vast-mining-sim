#ifndef INCLUDE_LOGGER_H_
#define INCLUDE_LOGGER_H_

#include <exception>
#include <fstream>
#include <memory>
#include <string>
#include <type_traits>

// Supported log levels (ordered by severity)
enum class LogLevel { INFO, DEBUG, WARNING, ERROR };

// Base logger interface for writing log messages to console
class Logger {
 public:
  virtual ~Logger() = default;
  virtual void Log(LogLevel level, const std::string& message);

 protected:
  // Converts LogLevel enum to string label (e.g., "INFO")
  std::string GetLogLevelString(LogLevel level) const;

  // Returns current timestamp formatted as YYYY-MM-DD HH:MM:SS
  std::string GetTimestamp() const;
};

// Logger that writes messages to a file
class FileLogger : public Logger {
 public:
  explicit FileLogger(std::string filename = "");
  ~FileLogger();

  void Log(LogLevel level, const std::string& message) override;

  // Returns a timestamp suitable for use in a filename
  static std::string GetFileNameTimestamp();

 private:
  std::ofstream file_;
};

// Global logger access and override
Logger& GetLogger();
void SetLogger(std::shared_ptr<Logger>);

// Convenience wrappers for logging at specific levels
void LogInfo(const std::string& message);
void LogDebug(const std::string& message);
void LogWarning(const std::string& message);
void LogError(const std::string& message);

// Logs and throws an exception of type ExceptionType
template <typename ExceptionType, typename = std::enable_if_t<std::is_base_of_v<
                                      std::exception, ExceptionType>>>
void LogAndThrowError(const std::string& message) {
  LogError(message);
  throw ExceptionType(message);
}

#endif  // INCLUDE_LOGGER_H_
