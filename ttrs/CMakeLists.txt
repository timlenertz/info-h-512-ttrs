project(ttrs CXX)
cmake_minimum_required(VERSION 3.1)

file(GLOB_RECURSE CPP_SRC "./src/*.cc")

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall -Wno-missing-braces")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lncurses")

add_executable(ttrs ${CPP_SRC})
