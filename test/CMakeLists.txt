enable_testing()

option(BUILD_GMOCK "Builds the googlemock subproject" OFF)
option(INSTALL_GTEST "Enable installation of googletest. (Projects embedding googletest may want to turn this OFF.)" OFF)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        6910c9d9165801d8827d628cb72eb7ea9dd538c5 # v1.16.0
)

FetchContent_MakeAvailable(googletest)

add_executable(test-main
    main.test.cpp)

target_link_libraries(test-main
    PRIVATE
        gtest_main)

include(GoogleTest)
gtest_discover_tests(test-main)