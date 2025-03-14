cmake_minimum_required(VERSION 3.30)

project(GameboyEmulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

link_directories(${CMAKE_SOURCE_DIR}/lib/raylib)
add_executable(${PROJECT_NAME} src/main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include/raylib)
target_link_libraries(${PROJECT_NAME} raylib gdi32 winmm)


