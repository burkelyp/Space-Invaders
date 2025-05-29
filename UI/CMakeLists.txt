cmake_minimum_required(VERSION 3.16)
    project(SpaceInvadersUI VERSION 1.0 LANGUAGES CXX)
    
    find_package(Qt6 6.9 REQUIRED COMPONENTS Core Widgets Gui OpenGL OpenGLWidgets Network) # Add other modules as needed
    
    qt_standard_project_setup()
    
    add_executable(SpaceInvadersUI SpaceInvadersUI.cpp KeyboardMapper.cpp GraphicsWindow.cpp)
    
    target_link_libraries(SpaceInvadersUI PRIVATE Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Network Qt6::OpenGL Qt6::OpenGLWidgets rt) # Link needed modules

    set_target_properties(SpaceInvadersUI PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
    )

add_compile_definitions(_POSIX_C_SOURCE=200112L)