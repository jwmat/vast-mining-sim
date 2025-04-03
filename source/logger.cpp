#include "logger.h"

void Logger::Init(std::string filename) {
  if (initialized_) return;

  if (filename.empty()) {
    auto now = std::chrono::system_clock::now();
    std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm* local_time = std::localtime(&time_t_now);

    std::stringstream timestamp;
    timestamp << std::put_time(local_time, "%Y-%m-%d_%H-%M-%S");
    filename = "log_" + timestamp.str() + ".txt";
  }

  // Create async logger with multi-sink (console + file)
  spdlog::init_thread_pool(8192, 1);  // queue size, thread count

  auto file_sink =
      std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true);
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

  std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
  auto logger = std::make_shared<spdlog::async_logger>(
      "async_logger", sinks.begin(), sinks.end(), spdlog::thread_pool(),
      spdlog::async_overflow_policy::block);

  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::info);  // default log level
  spdlog::set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

  initialized_ = true;
}
