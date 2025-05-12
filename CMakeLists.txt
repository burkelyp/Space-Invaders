cmake_minimum_required(VERSION 3.10)
# Define the project
project(SpaceInvadersProject LANGUAGES CXX) # Choose a name for your project

# --- Determine Platform ---
# Detect platform and define macro
if(WIN32)
    add_definitions(-DPLATFORM_WINDOWS)
endif()

# --- Create a Library for Your Emulator Code ---
# This bundles your core emulator logic so executables (like tests or the main app) can link against it.
# *** Make sure to list ALL necessary .cpp files for the emulator here ***
add_library(emulator_lib
    emulator.cpp
    initcpu.cpp 
    disassembler.cpp
    io_ports.cpp
    loadrom.cpp
    # access_mmap.cpp
    # Add any other core .cpp files needed by the emulator
)

# Tell CMake where to find headers for emulator_lib (cpu.h, emulator.h...)
# PUBLIC makes the include path available to emulator_lib AND anything linking to it.
target_include_directories(emulator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


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
