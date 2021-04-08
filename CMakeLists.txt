# Minimum CMake required
cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Project

project(clairvoyantedge-metadata CXX)


find_package(PkgConfig REQUIRED)


add_library(clairvoyantedge-metadata   src/redis-messages.cc)

add_subdirectory(test)


find_path(HIREDIS_HEADER hiredis)
target_include_directories(clairvoyantedge-metadata PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(clairvoyantedge-metadata PUBLIC ${HIREDIS_LIB})

find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(clairvoyantedge-metadata PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(clairvoyantedge-metadata PUBLIC ${REDIS_PLUS_PLUS_LIB})


set(GCC_FLAGS "-pthread -lstdc++fs")

target_link_libraries(clairvoyantedge-metadata  PUBLIC
                              ${GCC_FLAGS}
                              )

