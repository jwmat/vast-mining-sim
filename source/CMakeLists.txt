add_library(vast-mining-sim
    controller.cpp
    event.cpp
    logger.cpp
    mine_manager.cpp
    report.cpp
    truck_manager.cpp
    station_manager.cpp)

target_include_directories(vast-mining-sim
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(vast-mining-sim
    PRIVATE
        nlohmann_json::nlohmann_json)

add_executable(main
    main.cpp)

target_link_libraries(main
    PRIVATE
        vast-mining-sim)
