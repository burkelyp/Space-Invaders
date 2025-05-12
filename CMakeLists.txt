cmake_minimum_required(VERSION 3.10)
# Define the project
project(SpaceInvadersProject LANGUAGES CXX) # Choose a name for your project

include(FetchContent)

# Try to find SDL3 first
find_package(SDL3 QUIET CONFIG)

if (NOT SDL3_FOUND)
    message(STATUS "SDL3 not found, fetching and building it...")

    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-3.0.0  # or main if you want bleeding edge
    )
    FetchContent_MakeAvailable(SDL3)
endif()

# --- Determine Platform ---
# Detect platform and define macro
if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
endif()

add_library(emulator_lib
    # disassembler.cpp
    emulator.cpp
    initcpu.cpp
    io_ports.cpp
    loadrom.cpp
    access_mmap.cpp
    # Add any other core .cpp files needed by the emulator
)

# --- Define the Main Emulator Executable ---
add_executable(SpaceInvaders
    main.cpp
)

# Tell CMake where to find headers for emulator_lib (cpu.h, emulator.h...)
# PUBLIC makes the include path available to emulator_lib AND anything linking to it.
target_include_directories(emulator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link SDL3
target_link_libraries(SpaceInvaders PRIVATE SDL3::SDL3)

# Include paths (optional if already handled by the lib)
target_include_directories(SpaceInvaders PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


# --- Debug mode --- 
# To build with debug mode on:
#       cmake -DCMAKE_BUILD_TYPE=debug ./target/dir
if(CMAKE_BUILD_TYPE STREQUAL "debug")
    target_compile_definitions(SpaceInvaders PRIVATE DEBUG)
    add_compile_definitions(DEBUG)
endif()

# --- Define the Manual Test Executable ---
# Add the new executable target for manual tests
add_executable(ManualEmulatorTests
test/manual_tests.cpp # Path to your new file
)

# Link it against your emulator code library
target_link_libraries(ManualEmulatorTests PRIVATE emulator_lib)

# Make sure the test executable can find emulator.h, cpu.h etc.
# This might be redundant if the PUBLIC include from emulator_lib works, but ensures it.
target_include_directories(ManualEmulatorTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
