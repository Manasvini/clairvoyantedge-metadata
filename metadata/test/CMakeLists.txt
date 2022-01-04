# Minimum CMake required
cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Project

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
find_package(yaml-cpp REQUIRED)


include_directories(../src/)

add_executable(metadata-tester  
                                #../src/redis-connection.cc 
                                #test.cc
                                test_cluster.cc
                                test_main.cc
                                metadata-client-test.cc
                )
target_include_directories(metadata-tester PUBLIC ../redis-lib)
message("My test's working directory: ${CMAKE_CURRENT_BINARY_DIR}")



find_library(HIREDIS_LIB hiredis)
target_link_libraries(metadata-tester PUBLIC ${HIREDIS_LIB})

find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(metadata-tester PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(metadata-tester PUBLIC ${REDIS_PLUS_PLUS_LIB})


target_link_libraries(metadata-tester  PUBLIC
			      Threads::Threads	    	 
			      GTest::gtest 
                  protolib
                  clairvoyantedge-redislib
                    )

