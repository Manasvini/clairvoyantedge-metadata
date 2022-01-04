cmake_minimum_required(VERSION 3.19)
set(CMAKE_TOOLCHAIN_FILE "$ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_CXX_STANDARD 17)

#include(${CMAKE_TOOLCHAIN_FILE})
project(clairvoyantedge-metadata CXX)
find_package (glog CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_subdirectory(protos)
add_subdirectory(metadata)
