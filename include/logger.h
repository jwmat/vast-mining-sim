#ifndef INCLUDE_LOGGER_H_
#define INCLUDE_LOGGER_H_

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>
#include <string>

class Logger {
 public:
  static void Init(std::string filename = "");

  template <typename T>
  static void LogTrace(const T& msg) {
    spdlog::trace("{}", msg);
  }

  template <typename T>
  static void LogInfo(const T& msg) {
    spdlog::info("{}", msg);
  }

  template <typename T>
  static void LogWarning(const T& msg) {
    spdlog::warn("{}", msg);
  }

  template <typename T>
  static void LogError(const T& msg) {
    spdlog::error("{}", msg);
  }

  template <typename E = std::runtime_error, typename T>
  static void LogAndThrowError(const T& msg) {
    spdlog::critical("{}", msg);
    throw E(msg);
  }

 private:
  static inline bool initialized_ = false;
};

#endif  // INCLUDE_LOGGER_H_
