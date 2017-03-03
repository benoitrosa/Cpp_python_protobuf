cmake_minimum_required(VERSION 2.8)

project (cpp_client)


set(CXX g++)
add_definitions(-std=c++11) 


include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../messages/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB HDRS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HDRS} ${PROTO_SRCS} ${PROTO_HDRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})

