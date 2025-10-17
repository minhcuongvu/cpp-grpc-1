cmake_minimum_required(VERSION 3.10)

# Project name
project(Example CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Output directories
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Define directory for generated protobuf and gRPC files
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Find gRPC and protobuf packages
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Compile the Protobuf and gRPC files
set(PROTO_FILE ${CMAKE_SOURCE_DIR}/proto/helloworld.proto)

add_library(protolib ${PROTO_FILE})
target_link_libraries(protolib gRPC::grpc++)

target_include_directories(protolib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
# https://github.com/protocolbuffers/protobuf/blob/main/docs/cmake_protobuf_generate.md
protobuf_generate(TARGET protolib LANGUAGE cpp
PROTOC_OUT_DIR "${GENERATED_DIR}")
protobuf_generate(TARGET protolib LANGUAGE grpc 
GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc 
PROTOC_OUT_DIR "${GENERATED_DIR}"
PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}")

# Include the generated code directory
include_directories(${GENERATED_DIR} ${GENERATED_DIR}/proto)

# Create the executables
# add_executable(server ${CMAKE_SOURCE_DIR}/example_server.cc)
# target_link_libraries(server protolib)
add_executable(client ${CMAKE_SOURCE_DIR}/src/client.cc)
target_link_libraries(client protolib)

# Set the output directories for executables
# set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# see more examples in https://github.com/grpc/grpc/tree/master/examples/cpp
