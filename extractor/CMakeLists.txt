project(HelloWorld)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(.)
include_directories(../externals)

add_executable(hello_world main.cpp)
