cmake_minimum_required(VERSION 3.25)

project(Sentinel VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(
    "$<$<CONFIG:Debug>:DEBUG>"
    "$<$<CONFIG:RelWithDebInfo>:RELEASE>"
    "$<$<CONFIG:Release>:DIST>"
)

find_package(SDL3 CONFIG REQUIRED)

file(
    GLOB_RECURSE CORE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine/*.h"
)

add_library(Core STATIC ${CORE_SOURCES})
target_include_directories(Core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/engine")
target_link_libraries(Core PUBLIC SDL3::SDL3)

file(
    GLOB_RECURSE GAME_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/*.h"
)

add_executable(Sentinel src/main.cpp ${GAME_SOURCES})
target_link_libraries(Sentinel PRIVATE Core)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT Sentinel)
