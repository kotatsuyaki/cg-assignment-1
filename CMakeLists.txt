cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(proj VERSION 1.0)

add_executable(proj src/main.cpp src/glad.c src/Matrices.cpp)
set_property(TARGET proj PROPERTY CXX_STANDARD 14)
include_directories(proj include)

# Use -DBUILD_GLFW=true to force bundled glfw
if(WIN32 OR BUILD_GLFW)
    # Compile glfw from submodule and link it
    add_subdirectory(./glfw)
    include_directories(./glfw/include)
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
