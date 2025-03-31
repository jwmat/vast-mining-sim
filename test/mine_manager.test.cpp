#include "mine_manager.h"

#include <gtest/gtest.h>

// Verifies that generated mining durations fall within configured bounds
TEST(TestMineManager, DurationWithinExpectedRange) {
  auto time = MineManager::Duration();
  EXPECT_GE(time, MineManager::kMinDuration);
  EXPECT_LE(time, MineManager::kMaxDuration);
}
