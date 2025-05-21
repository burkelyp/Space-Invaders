cmake_minimum_required(VERSION 3.10)
project(SpaceInvadersProject LANGUAGES CXX)

# --- Platform Detection ---
if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS -DUNICODE -D_UNICODE)

    # Validate SDL2 paths passed from the build system
    if(NOT SDL2_INCLUDE_DIR OR NOT SDL2_LIBRARY OR NOT SDL2MAIN_LIBRARY)
        message(FATAL_ERROR "Please define SDL2_INCLUDE_DIR, SDL2_LIBRARY, and SDL2MAIN_LIBRARY on Windows.")
    endif()

else()
    # Use pkg-config to find SDL2 on Linux/macOS
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
endif()

# --- Core Emulator Library ---
add_library(emulator_lib
    emulator.cpp
    initcpu.cpp
    io_ports.cpp
    loadrom.cpp
    access_mmap.cpp
    sound.cpp
)
target_include_directories(emulator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Main Executable ---
add_executable(SpaceInvaders main.cpp)

# Include project headers
target_include_directories(SpaceInvaders PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# SDL2 setup
if(WIN32)
    target_include_directories(SpaceInvaders PRIVATE ${SDL2_INCLUDE_DIR})
    target_link_libraries(SpaceInvaders PRIVATE emulator_lib ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY})
else()
    target_include_directories(SpaceInvaders PRIVATE ${SDL2_INCLUDE_DIRS})
    link_directories(${SDL2_LIBRARY_DIRS})
    target_link_libraries(SpaceInvaders PRIVATE emulator_lib ${SDL2_LIBRARIES})
endif()

# --- Debug Mode ---
if(CMAKE_BUILD_TYPE STREQUAL "debug")
    target_compile_definitions(SpaceInvaders PRIVATE DEBUG)
endif()

# --- Manual Test Executable ---
add_executable(ManualEmulatorTests test/manual_tests.cpp)
target_link_libraries(ManualEmulatorTests PRIVATE emulator_lib)
target_include_directories(ManualEmulatorTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
