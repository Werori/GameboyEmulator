cmake_minimum_required(VERSION 3.30)

project(GameboyEmulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

if(MSVC)
    set(RAYLIB_LIB raylib.lib)
else()
    set(RAYLIB_LIB raylib.a)
endif()

link_directories(${CMAKE_SOURCE_DIR}/lib/raylib)
add_executable(${PROJECT_NAME} src/main.cpp src/Gameboy.cpp
  src/GameboyMemory.cpp src/Cpu.cpp src/Registers.cpp src/Range.cpp src/Ppu.cpp
  src/Timers.cpp)

include_directories(${CMAKE_SOURCE_DIR}/include/raylib)
include_directories(${CMAKE_SOURCE_DIR}/include/gameboy)

target_link_libraries(${PROJECT_NAME} ${RAYLIB_LIB} gdi32 winmm)


