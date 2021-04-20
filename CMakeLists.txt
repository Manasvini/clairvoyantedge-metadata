# Minimum CMake required
cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Project

include(FindPkgConfig)

project(clairvoyantedge-metadata CXX)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -frtti")
find_package(PkgConfig REQUIRED)
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION grpc++_reflection)

if(CMAKE_CROSSCOMPILING)
    find_program(_PROTOUF_PROTOC protoc)
else()
    set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
endif()
set(_GRPC_GRPCPP gRPC::grpc++)
if(CMAKE_CROSSCOMPILING)
    find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)
else()
    set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
endif()


get_filename_component(clairvoyantedge_proto "proto/clairvoyantedge.proto" ABSOLUTE)
get_filename_component(clairvoyantedge_proto_path "${clairvoyantedge_proto}" PATH)

# Generated sources
set(clairvoyantedge_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/clairvoyantedge.pb.cc")
set(clairvoyantedge_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/clairvoyantedge.pb.h")
set(clairvoyantedge_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/clairvoyantedge.grpc.pb.cc")
set(clairvoyantedge_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/clairvoyantedge.grpc.pb.h")



add_custom_command(
      OUTPUT "${clairvoyantedge_proto_srcs}" "${clairvoyantedge_proto_hdrs}" "${clairvoyantedge_grpc_srcs}" "${clairvoyantedge_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS   --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
             --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
            -I "${clairvoyantedge_proto_path}"
             --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
                "${clairvoyantedge_proto}" 
                 DEPENDS "${clairvoyantedge_proto}")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_executable(clairvoyantedge-metadata   src/redis-messages.cc 
                                       src/redis-connection.cc
                                       "${clairvoyantedge_proto_srcs}"
                                      "${clairvoyantedge_grpc_srcs}"
                                       src/metadata-server-impl.h
                                       src/metadata-server-impl.cc
                                       src/main.cc
                                       src/metadata-task.cc
)

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
                              ${_REFLECTION}
                              ${_GRPC_GRPCPP}
                              ${_PROTOBUF_LIBPROTOBUF}
                              unagi
                              hummus
                              yaml-cpp
                     )
