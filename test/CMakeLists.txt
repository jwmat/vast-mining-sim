enable_testing()

function(add_test_executable target)
  add_executable(${target} ${ARGN})

  target_link_libraries(${target}
    PRIVATE
      GTest::gtest
      GTest::gtest_main
      vast-mining-sim)

  gtest_discover_tests(${target}
    EXTRA_ARGS --gtest_color=yes)
endfunction()


add_test_executable(test-controller
  controller.test.cpp)

add_test_executable(test-metrics
  metrics.test.cpp)
