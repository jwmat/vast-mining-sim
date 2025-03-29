enable_testing()

option(BUILD_GMOCK OFF)
option(INSTALL_GTEST OFF)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 6910c9d9165801d8827d628cb72eb7ea9dd538c5 # v1.16.0
)

FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_executable(test-main 
  main.test.cpp)

target_link_libraries(test-main 
    gtest_main)

gtest_discover_tests(test-main)

add_executable(test-random 
  random.test.cpp)

target_link_libraries(test-random
  gtest_main
  vast-mining-sim)

gtest_discover_tests(test-random)

add_executable(test-truck-manager
  truck_manager.test.cpp)

target_link_libraries(test-truck-manager
  PRIVATE
    gtest_main vast-mining-sim)

gtest_discover_tests(test-truck-manager)

add_executable(test-station-manager
  station_manager.test.cpp)

target_link_libraries(test-station-manager
  gtest_main
  vast-mining-sim)

gtest_discover_tests(test-station-manager)
