cmake_minimum_required(VERSION 3.10)
# Define the project
project(SpaceInvadersProject LANGUAGES CXX) # Choose a name for your project
# --- Create a Library for Your Emulator Code ---
# This bundles your core emulator logic so executables (like tests or the main app) can link against it.
# *** Make sure to list ALL necessary .cpp files for the emulator here ***
add_library(emulator_lib
    emulator.cpp
    #cpu.cpp
    disassembler.cpp
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


# --- Optional: CTest Integration ---
# Uncomment these lines if you want to use `make test` or `ctest` later
# Ensure include(CTest) and enable_testing() only appear ONCE in your entire CMakeLists.txt
# include(CTest)
# enable_testing()
# add_test(NAME RunManualTests COMMAND ManualEmulatorTests)

# --- NOTE: ---
# Make sure you have these lines near the TOP of your CMakeLists.txt file:
# cmake_minimum_required(VERSION 3.14) # Or your preferred minimum version
# project(SpaceInvadersEmulator LANGUAGES CXX)
# set(CMAKE_CXX_STANDARD 11) # Or 14, 17, 20
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
