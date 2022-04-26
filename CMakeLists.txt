cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(proj VERSION 1.0)

add_executable(proj
    src/main.cpp
    src/matrix.cpp
    src/model.cpp
    src/prompt.cpp
    src/scene.cpp
    src/shader.cpp
    src/window.cpp
    src/transform/mvp.cpp
    src/transform/projection.cpp
    src/transform/rotation.cpp
    src/transform/viewer.cpp
    3rdparty/glad/glad.c
)
set_property(TARGET proj PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

include_directories(3rdparty)
include_directories(3rdparty/nativefiledialog-extended/src/include)

add_subdirectory(3rdparty/tinyobjloader)

option(NFD_PORTAL "Use xdg-desktop-portal instead of GTK" ON)
add_subdirectory(3rdparty/nativefiledialog-extended)

# Use -DSYSTEM_GLFW=true to use glfw from the system
if(SYSTEM_GLFW)
    find_package(glfw3 3.2 REQUIRED)
    target_link_libraries(proj glfw dl tinyobjloader nfd)
else()
    # Compile glfw from submodule and link it
    add_subdirectory(3rdparty/glfw)
    include_directories(3rdparty/glfw/include)
    target_link_libraries(proj glfw ${GLFW_LIBRARIES} tinyobjloader nfd)
endif()

if(NOT WIN32)
    add_compile_options(-Wall -Wextra)
    install(TARGETS proj DESTINATION bin)
endif()

# Export compile commands for clangd
if(DEFINED ENV{CMAKE_EXPORT_COMPILE_COMMANDS})
    message(STATUS "Exporting compile commands")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()
