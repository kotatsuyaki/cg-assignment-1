cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(proj VERSION 1.0)

include_directories(3rdparty)
add_executable(proj
    src/main.cpp
    src/matrix.cpp
    src/window.cpp
    3rdparty/glad/glad.c
)
set_property(TARGET proj PROPERTY CXX_STANDARD 17)

# Use -DBUILD_GLFW=true to force bundled glfw
if(WIN32 OR BUILD_GLFW)
    # Compile glfw from submodule and link it
    add_subdirectory(3rdparty/glfw)
    include_directories(3rdparty/glfw/include)
    target_link_libraries(proj glfw ${GLFW_LIBRARIES})
else()
    find_package(glfw3 3.2 REQUIRED)
    target_link_libraries(proj glfw dl)
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
