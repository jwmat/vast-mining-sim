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
      --filter=-legal/copyright,-build/include_subdir,-build/namespaces
      ${ALL_CXX_SOURCE_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running cpplint")
endif()

find_package(GTest CONFIG)

if (NOT GTest_FOUND)
  option(BUILD_GMOCK OFF)
  option(INSTALL_GTEST OFF)

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
  include(FetchContent)

  FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
  )

  FetchContent_MakeAvailable(nlohmann_json)
endif()
