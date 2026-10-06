cmake_minimum_required(VERSION 3.16)
project(DijkstraVisual LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Use an installed SFML 2.6 if there is one, otherwise download it.
find_package(SFML 2.6 COMPONENTS graphics window system QUIET)
if(NOT SFML_FOUND)
    include(FetchContent)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
        GIT_SHALLOW ON)
    FetchContent_MakeAvailable(SFML)
endif()

add_executable(dijkstra-visual
    src/main.cpp
    src/App.cpp
    src/Dijkstra.cpp
    src/Graph.cpp
    src/GraphIO.cpp
    src/Renderer.cpp)

target_include_directories(dijkstra-visual PRIVATE src)
target_link_libraries(dijkstra-visual PRIVATE sfml-graphics sfml-window sfml-system)
target_compile_definitions(dijkstra-visual PRIVATE DIJKSTRA_RESOURCE_DIR="${CMAKE_SOURCE_DIR}")

if(MSVC)
    target_compile_options(dijkstra-visual PRIVATE /W4 /permissive-)
else()
    target_compile_options(dijkstra-visual PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Put the assets and sample data next to the executable so it runs from anywhere.
add_custom_command(TARGET dijkstra-visual POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:dijkstra-visual>/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:dijkstra-visual>/data)
