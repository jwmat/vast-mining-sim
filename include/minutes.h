#ifndef INCLUDE_MINUTES_H_
#define INCLUDE_MINUTES_H_

#include <chrono>

// Alias for time durations expressed in minutes
using minutes_t = std::chrono::minutes;

// Enables usage of literals like 5min, 30min, etc.
using namespace std::chrono_literals;

#endif  // INCLUDE_MINUTES_H_
