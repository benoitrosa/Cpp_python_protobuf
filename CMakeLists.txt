cmake_minimum_required(VERSION 2.8)


set(CMAKE_BUILD_TYPE Release)

list( APPEND CMAKE_CXX_FLAGS "-Wno-deprecated-declarations ${CMAKE_CXX_FLAGS}")

project (Protobuf_cpp_python CXX)

add_subdirectory(cpp_server)
