# Minimum CMake required
cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# Project

include(FindPkgConfig)


SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -frtti")
add_executable(data-uploader  data_uploader.cc) 

target_include_directories(data-uploader  PUBLIC ../redis-lib)

set(GCC_FLAGS "-pthread -lstdc++fs")
target_link_libraries(data-uploader  PUBLIC
                              ${GCC_FLAGS}
                              protolib
                              unagi)
