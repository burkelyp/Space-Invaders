cmake_minimum_required(VERSION 3.10)
project(SpaceInvadersProject LANGUAGES CXX)

# --- Determine Platform ---
if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
endif()

# --- SDL2 Setup ---
# Manually specify SDL2 include and lib paths on Windows.
# On Linux, we assume libsdl2-dev is installed to system paths.
if(WIN32)
    if(NOT SDL2_INCLUDE_DIR OR NOT SDL2_LIBRARY OR NOT SDL2MAIN_LIBRARY)
        message(FATAL_ERROR "Please set SDL2_INCLUDE_DIR, SDL2_LIBRARY, and SDL2MAIN_LIBRARY for Windows builds.")
    endif()
endif()

# --- Emulator Library ---
add_library(emulator_lib
    emulator.cpp
    initcpu.cpp
    io_ports.cpp
    loadrom.cpp
    access_mmap.cpp
)

target_include_directories(emulator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Main Executable ---
add_executable(SpaceInvaders
    main.cpp
)

target_include_directories(SpaceInvaders PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(SpaceInvaders PRIVATE emulator_lib)

if(WIN32)
    include_directories(${SDL2_INCLUDE_DIR})
    target_link_libraries(SpaceInvaders PRIVATE ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY})
else()
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(SpaceInvaders PRIVATE ${SDL2_LIBRARIES})
endif()

# --- Debug Mode ---
if(CMAKE_BUILD_TYPE STREQUAL "debug")
    target_compile_definitions(SpaceInvaders PRIVATE DEBUG)
    add_compile_definitions(DEBUG)
endif()

# --- Manual Test Executable ---
add_executable(ManualEmulatorTests
    test/manual_tests.cpp
)
target_link_libraries(ManualEmulatorTests PRIVATE emulator_lib)
target_include_directories(ManualEmulatorTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})