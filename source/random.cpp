#include "random.h"

std::default_random_engine Random::engine_(SEED);

minutes_t Random::Duration() {
  std::uniform_int_distribution<uint64_t> dist(kMinDuration.count(),
                                               kMaxDuration.count());
  return minutes_t(dist(engine_));
}
