# Minimum CMake required
cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

# Project

include(FindPkgConfig)


SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -frtti")
add_library(clairvoyantedge-redislib   redis-messages.cc 
                                       redis-connection.cc
)


find_path(HIREDIS_HEADER hiredis)
target_include_directories(clairvoyantedge-redislib PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(clairvoyantedge-redislib PUBLIC ${HIREDIS_LIB})

find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(clairvoyantedge-redislib PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(clairvoyantedge-redislib PUBLIC ${REDIS_PLUS_PLUS_LIB})


set(GCC_FLAGS "-pthread -lstdc++fs")
target_link_libraries(clairvoyantedge-redislib  PUBLIC
                              ${GCC_FLAGS}
                     )


target_include_directories(clairvoyantedge-redislib PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/redis-lib)
