enable_testing()

function(add_test_executable target)
  add_executable(${target} ${ARGN})

  target_link_libraries(${target} 
    PRIVATE
      GTest::gtest
      GTest::gtest_main
      vast-mining-sim)

  gtest_discover_tests(${target})
endfunction()

add_test_executable(test-controller
  controller.test.cpp)

add_test_executable(test-logger
  logger.test.cpp)

add_test_executable(test-station-manager
  station_manager.test.cpp)

add_test_executable(test-mine-manager 
  mine_manager.test.cpp)

add_test_executable(test-truck-manager
  truck_manager.test.cpp)
