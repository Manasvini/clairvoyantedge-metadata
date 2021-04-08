# Minimum CMake required
cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Project

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

include_directories(../src/)

add_executable(metadata-tester  ../src/redis-messages.cc 
                                test.cc
                                test_main.cc
                )

set(GCC_FLAGS "-lpthread -lgtest")
target_link_libraries(metadata-tester  PUBLIC gtest 
                              ${GCC_FLAGS})




