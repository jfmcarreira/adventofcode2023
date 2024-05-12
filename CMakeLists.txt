cmake_minimum_required(VERSION 3.27)
project(adventofcode CXX)

set(CMAKE_C_COMPILER /opt/homebrew/Cellar/llvm@16/16.0.6/bin/clang)
set(CMAKE_CXX_COMPILER /opt/homebrew/Cellar/llvm@16/16.0.6/bin/clang++)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_subdirectory(03)
add_subdirectory(04)
add_subdirectory(05)
add_subdirectory(06)
add_subdirectory(07)
add_subdirectory(08)
add_subdirectory(09)
add_subdirectory(10)
add_subdirectory(11)
add_subdirectory(12)