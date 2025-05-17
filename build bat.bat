@echo off
REM Set working directory to script location
cd /d "%~dp0"

REM Create build directory if it doesn't exist
if not exist build (
    mkdir build
)
cd build

REM Run CMake to generate Visual Studio build files
cmake .. -G "Visual Studio 17 2022" ^
  -DSDL2_INCLUDE_DIR="C:\sdl2\include" ^
  -DSDL2_LIBRARY="C:\sdl2\lib\x64\SDL2.lib" ^
  -DSDL2MAIN_LIBRARY="C:\sdl2\lib\x64\SDL2main.lib"

REM Build the project using MSBuild (Release config)
cmake --build . --config Release

REM Copy SDL2.dll into the output directory
copy "C:\sdl2\lib\x64\SDL2.dll" .\Release\ >nul

REM Run the emulator with the ROM path
cd Release
SpaceInvaders.exe ..\..\roms\invaders.rom

REM Keep the window open after execution
echo.
echo [Program finished. Press any key to close.]
pause >nul
