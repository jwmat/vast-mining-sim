file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")

add_library(vast-mining-sim
  ${HEADER_FILES} # For MSVC
    controller.cpp
    event.cpp
    logger.cpp
    report.cpp)

target_include_directories(vast-mining-sim
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(vast-mining-sim
    PUBLIC
        spdlog::spdlog
    PRIVATE
        nlohmann_json::nlohmann_json)

add_executable(main
    main.cpp)

target_link_libraries(main
    PRIVATE
        vast-mining-sim)
