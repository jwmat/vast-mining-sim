include(FetchContent)

find_program(CPPLINT cpplint)

if(CPPLINT)
  file(GLOB_RECURSE ALL_CXX_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h")

  add_custom_target(cpplint
    COMMAND ${CPPLINT}
      --linelength=100
      --filter=-legal/copyright,-build/include_subdir,-build/namespaces,-build/c++11
      ${ALL_CXX_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running cpplint")
endif()

find_package(GTest CONFIG)

if (NOT GTest_FOUND)
  option(BUILD_GMOCK OFF)
  option(INSTALL_GTEST OFF)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  include(FetchContent)

  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )

  FetchContent_MakeAvailable(googletest)

  add_library(GTest::gtest_main ALIAS gtest_main)
endif()
include(GoogleTest)

find_package(nlohmann_json CONFIG)

if (NOT nlohmann_json_FOUND)
  message(STATUS "Downloading nlohmann_json...")

  FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
  )

  FetchContent_MakeAvailable(nlohmann_json)
endif()

# Find clang-format if available
find_program(CLANG_FORMAT_EXE NAMES clang-format)

if(CLANG_FORMAT_EXE)
  file(GLOB_RECURSE ALL_SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

  add_custom_target(format
    COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
    COMMENT "Running clang-format on source files"
  )
endif()

find_package(spdlog CONFIG)

if (NOT spdlog_FOUND)
  message(STATUS "Downloading spdlog...")
  FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.2
  )

  FetchContent_MakeAvailable(spdlog)
endif()
