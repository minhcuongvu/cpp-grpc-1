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

# Directory for generated protobuf and gRPC files
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

# Find gRPC and protobuf packages
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# Automatically find all .proto files in proto/ and subfolders
file(GLOB_RECURSE PROTO_FILES
    "${CMAKE_SOURCE_DIR}/proto/*.proto"
    "${CMAKE_SOURCE_DIR}/proto/extra/*.proto"
)

# Library for proto code
add_library(protolib ${PROTO_FILES})
target_link_libraries(protolib gRPC::grpc++)
target_include_directories(protolib PUBLIC ${GENERATED_DIR}/proto ${GENERATED_DIR}/proto/extra)

# gRPC plugin
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)

# Generate protobuf C++ code
protobuf_generate(TARGET protolib LANGUAGE cpp
    PROTOC_OUT_DIR "${GENERATED_DIR}"
)

# Generate gRPC C++ code
protobuf_generate(TARGET protolib LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PROTOC_OUT_DIR "${GENERATED_DIR}"
    PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
)

# Include the generated code directories
include_directories(${GENERATED_DIR})

# Add the client executable
add_executable(client ${CMAKE_SOURCE_DIR}/src/client.cc)
target_link_libraries(client protolib)
set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Optionally: add server or other executables here
# add_executable(server ${CMAKE_SOURCE_DIR}/src/server.cc)
# target_link_libraries(server protolib)
# set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
