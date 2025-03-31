#ifndef INCLUDE_HEAP_H_
#define INCLUDE_HEAP_H_

#include <functional>
#include <queue>
#include <utility>
#include <vector>

#include "minutes.h"

// A thin wrapper around std::priority_queue that provides a min-heap
// and a convenient take() method for extracting the top element.
class MinHeap
    : public std::priority_queue<std::pair<minutes_t, size_t>,
                                 std::vector<std::pair<minutes_t, size_t>>,
                                 std::greater<>> {
 public:
  using type = std::pair<minutes_t, size_t>;

  // Removes and returns the top (minimum) element from the heap
  type take() {
    auto t = top();
    pop();
    return t;
  }
};

#endif  // INCLUDE_HEAP_H_
