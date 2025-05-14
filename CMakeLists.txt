cmake_minimum_required(VERSION 3.10)
project(SpaceInvadersProject LANGUAGES CXX)

# Use pkg-config to find SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Add include and lib directories
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})

# Core emulator library
add_library(emulator_lib
    emulator.cpp
    initcpu.cpp
    io_ports.cpp
    loadrom.cpp
    access_mmap.cpp
)

target_include_directories(emulator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Main SDL2 executable
add_executable(SpaceInvaders main.cpp)
target_include_directories(SpaceInvaders PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SpaceInvaders PRIVATE emulator_lib ${SDL2_LIBRARIES})

# Optional: enable debug macros
if(CMAKE_BUILD_TYPE STREQUAL "debug")
    target_compile_definitions(SpaceInvaders PRIVATE DEBUG)
endif()

# Manual test target
add_executable(ManualEmulatorTests test/manual_tests.cpp)
target_link_libraries(ManualEmulatorTests PRIVATE emulator_lib)
target_include_directories(ManualEmulatorTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
