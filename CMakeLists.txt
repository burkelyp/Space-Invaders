cmake_minimum_required(VERSION 3.10)
project(SpaceInvadersProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Determine Platform ---
if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
endif()

# --- Add Core Emulator Library ---
add_library(emulator_lib
    emulator.cpp
    initcpu.cpp
    io_ports.cpp
    loadrom.cpp
    access_mmap.cpp
)

# --- SDL3 Setup for Windows ---
if(WIN32)
    include_directories("C:/sdl3/include")
    set(SDL3_LIB_PATH "C:/sdl3/lib/x64/SDL3.lib")
    target_link_libraries(emulator_lib PUBLIC "${SDL3_LIB_PATH}")
endif()

# --- Main Executable ---
add_executable(SpaceInvaders
    main.cpp
)

# --- Link Libraries ---
target_link_libraries(SpaceInvaders PRIVATE emulator_lib)

# --- Include Path for Main ---
target_include_directories(SpaceInvaders PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# --- Debug Mode: Manual Test Executable ---
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(SpaceInvaders PRIVATE DEBUG)
    add_compile_definitions(DEBUG)

    add_executable(ManualEmulatorTests
        test/manual_tests.cpp
    )

    target_link_libraries(ManualEmulatorTests PRIVATE emulator_lib)
    target_include_directories(ManualEmulatorTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Optional Testing Support
# include(CTest)
# enable_testing()
# add_test(NAME RunManualTests COMMAND ManualEmulatorTests)
