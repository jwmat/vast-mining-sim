#include "random.h"

#include <gtest/gtest.h>

TEST(TestRandom, Interval) {
  auto time = Random::Duration();
  EXPECT_GE(time, Random::kMinDuration);
  EXPECT_LE(time, Random::kMaxDuration);
}
