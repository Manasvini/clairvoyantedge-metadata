# Minimum CMake required
cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# Project

include(FindPkgConfig)
find_package(yaml-cpp CONFIG REQUIRED)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -frtti")
add_executable(clairvoyantedge-metadata  metadata-server-impl.h
                                         metadata-server-impl.cc
                                         main.cc
                                         metadata-task.cc
                                         metadata-task-manager.cc
                                         nearest-neighbor-info.cc
                                         nearestnodes-task.cc 
                                         metadata-config-helper.cc
)

target_include_directories(clairvoyantedge-metadata  PUBLIC ../redis-lib)

set(GCC_FLAGS "-pthread -lstdc++fs")
target_link_libraries(clairvoyantedge-metadata  PUBLIC
                              ${GCC_FLAGS}
                              protolib
                              unagi
                              clairvoyantedge-redislib
                              glog::glog
                              yaml-cpp
                     )




