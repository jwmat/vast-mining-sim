#ifndef INCLUDE_RANDOM_H_
#define INCLUDE_RANDOM_H_

#include <chrono>  // NOLINT(build/c++11)
#include <random>

#define SEED 0xBEEF

using minutes_t = std::chrono::minutes;
using namespace std::chrono_literals;

// Generates a random duration between 1-5 hrs (per prompt)
class Random {
 public:
  static constexpr minutes_t kMinDuration = 60min;
  static constexpr minutes_t kMaxDuration = 300min;

  static minutes_t Duration();

 private:
  static std::default_random_engine engine_;
};

#endif  // INCLUDE_RANDOM_H_
