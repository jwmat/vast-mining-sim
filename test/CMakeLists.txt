enable_testing()

function(add_test_executable target)
  add_executable(${target} ${ARGN})

  target_link_libraries(${target}
    PRIVATE
      GTest::gtest
      GTest::gtest_main
      vast-mining-sim)
  
  if(NOT WIN32)
    set_target_properties(${target} PROPERTIES
      BUILD_RPATH "$<TARGET_FILE_DIR:GTest::gtest>"
      INSTALL_RPATH "$<TARGET_FILE_DIR:GTest::gtest>"
    )
  endif()

  add_test(NAME ${target}
         COMMAND $<TARGET_FILE:${target}> --gtest_color=yes)
endfunction()


add_test_executable(test-controller
  controller.test.cpp)

add_test_executable(test-metrics
  metrics.test.cpp)
