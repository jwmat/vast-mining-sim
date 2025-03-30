#ifndef INCLUDE_LOGGER_H_
#define INCLUDE_LOGGER_H_

#include <fstream>
#include <memory>
#include <string>

enum class LogLevel { INFO, DEBUG, WARNING, ERROR };

class Logger {
 public:
  virtual void Log(LogLevel level, const std::string& message);

 protected:
  std::string GetLogLevelString(LogLevel level) const;
  std::string GetTimestamp() const;
};

class FileLogger : public Logger {
 public:
  explicit FileLogger(std::string filename = "");
  ~FileLogger();

  void Log(LogLevel level, const std::string& message) override;

 private:
  std::ofstream file_;
};

Logger& GetLogger();
void SetLogger(std::shared_ptr<Logger>);

void LogInfo(const std::string& message);
void LogDebug(const std::string& message);
void LogWarning(const std::string& message);
void LogError(const std::string& message);

#endif  // INCLUDE_LOGGER_H_
