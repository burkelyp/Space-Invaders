cmake_minimum_required(VERSION 3.10)
# Define the project
project(SpaceInvadersProject LANGUAGES CXX) # Choose a name for your project

# --- Determine Platform ---
# Detect platform and define macro
if(_WIN64)
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

# Tell CMake where to find headers for emulator_lib (cpu.h, emulator.h...)
# PUBLIC makes the include path available to emulator_lib AND anything linking to it.
target_include_directories(emulator_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- Define the Main Emulator Executable ---
add_executable(SpaceInvaders
    main.cpp
)

# Find SDL3 manually
include_directories(/usr/local/include/SDL3)
link_directories(/usr/local/lib)

# Link it with the emulator library
target_link_libraries(SpaceInvaders PRIVATE emulator_lib SDL3)

# Include paths (optional if already handled by the lib)
target_include_directories(SpaceInvaders PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})


# --- Debug mode --- 
# To build with debug mode on:
#       cmake -DCMAKE_BUILD_TYPE=debug ./target/dir
if(CMAKE_BUILD_TYPE STREQUAL "debug")
    target_compile_definitions(SpaceInvaders PRIVATE DEBUG)
    add_compile_definitions(DEBUG)
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
endif()


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