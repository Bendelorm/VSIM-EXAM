cmake_minimum_required(VERSION 3.19)
project(QtVulkan LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
#find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

qt_standard_project_setup()

add_executable(QtVulkan
    main.cpp
    Renderer.h Renderer.cpp
    Utilities.h
    Vertex.h
    UI/MainWindow.h UI/MainWindow.cpp UI/MainWindow.ui
    logger.h logger.cpp
    macros.h
    core.h
    SoundTester.h SoundTester.cpp

    camera.h camera.cpp
    Components/Components.h
    Entities/Entity.h Entities/Entity.cpp
    Systems/System.h Systems/System.cpp
    Input.h
    quadtree.h quadtree.cpp
    engineinit.h engineinit.cpp
    Shaders/Shader.frag
    Shaders/Shader.vert
    Shaders/frag.spv
    Shaders/vert.spv
    Shaders/PhongShader.frag
    Shaders/PhongShader.vert
    Shaders/TraceShader.vert
    Shaders/TraceShader.frag
    registry.h
    entityManager.h
    Systems/inputsystem.h Systems/inputsystem.cpp
    Systems/FileSystem.h Systems/FileSystem.cpp
    magic_enum.hpp
    Widgets/widgetfile.cpp Widgets/widgetfile.h Widgets/widgetsquareimage.cpp Widgets/widgetsquareimage.h
    UI/widgetfile.ui
    Systems/CameraSystem.h Systems/CameraSystem.cpp
    Systems/RenderSystem.h Systems/RenderSystem.cpp
    TransformManager.h TransformManager.cpp
    TxtToObj.h TxtToObj.cpp
    Systems/PhysicsSystem.h Systems/PhysicsSystem.cpp
)

# OpenAL / Sound
target_include_directories(QtVulkan PRIVATE $ENV{OPENAL_HOME}/include)
target_link_libraries(QtVulkan PRIVATE $ENV{OPENAL_HOME}/libs/Win64/OpenAL32.lib) # could be OpenAL32.lib)
# Copy the DLL after build
# This makes the .exe use the correct .dll from the library we compile to
# (the "COMMAND echo" only works on Windows)
add_custom_command(TARGET QtVulkan POST_BUILD
    #COMMAND echo Copying OpenAL32.dll...
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $ENV{OPENAL_HOME}/bin/Win64/OpenAL32.dll
        $<TARGET_FILE_DIR:QtVulkan>)

if(MSVC)
    # Required for proper C++ exception handling with MSVC
    target_compile_options(QtVulkan PRIVATE /EHsc)

endif()


target_link_libraries(QtVulkan

    PRIVATE
            Qt6::Widgets
            Qt6::Core
            Vulkan::Vulkan
)

include(GNUInstallDirs)
install(TARGETS QtVulkan
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
