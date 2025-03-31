#include "mine_manager.h"

#include "logger.h"

// Initialize the RNG with a fixed seed for reproducibility
std::default_random_engine MineManager::engine_(RANDOM_SEED);

MineManager::MineManager() {
  LogInfo("Simulation RNG seed: " + std::to_string(RANDOM_SEED));
}

// Logs a mining event for the given truck, starting at the given time and
// lasting for the specified duration. Returns a reference to the event
MinHeap::type MineManager::MineTruck(size_t truck_id, minutes_t start_time,
                                     minutes_t mine_time) {
  LogEvent({EventType::Mine, truck_id, std::nullopt, start_time,
            start_time + mine_time});
  return {start_time + mine_time, 0};
}

// Returns a random mining duration between kMinDuration and kMaxDuration.*
// Assumes uniform distribution
minutes_t MineManager::Duration() {
  std::uniform_int_distribution<uint64_t> dist(kMinDuration.count(),
                                               kMaxDuration.count());
  return minutes_t(dist(engine_));
}
