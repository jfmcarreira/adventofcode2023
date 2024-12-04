cmake_minimum_required(VERSION 3.27)

set(YEAR_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )

set(CMAKE_C_COMPILER /opt/homebrew/Cellar/llvm/19.1.0/bin/clang)
set(CMAKE_CXX_COMPILER /opt/homebrew/Cellar/llvm/19.1.0/bin/clang++)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2c -Wall")

add_subdirectory(01)
add_subdirectory(02)
add_subdirectory(03)