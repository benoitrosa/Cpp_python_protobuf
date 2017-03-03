cmake_minimum_required(VERSION 2.8)

project (cpp_server)


set(CXX g++)
add_definitions(-std=c++11) 


include_directories(${CMAKE_CURRENT_SOURCE_DIR})


find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/kinMsg.proto)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB HDRS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HDRS} ${PROTO_SRCS} ${PROTO_HDRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROTOBUF_LIBRARIES})


## generate python protobuf (the default cmake command does not work) ##
set(PROTO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/kinMsg.proto)
set(OUTDIR ${CMAKE_CURRENT_SOURCE_DIR}/../python_client/)

get_filename_component(ABS_FIL ${PROTO_FILE} ABSOLUTE)
get_filename_component(ABS_PATH ${ABS_FIL} PATH)
get_filename_component(FIL_WE ${PROTO_FILE} NAME_WE)
set(OUT_PY ${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}_pb2.py)

list(APPEND ${SRCS} "${CMAKE_CURRENT_BINARY_DIR}/${FIL_WE}_pb2.py")

set(ARGS "-I=${CMAKE_CURRENT_SOURCE_DIR} --python_out ${OUTDIR} --proto_path ${PROTOBUF_INCLUDE_DIRS}")

set(PROTOBUF_COMMAND "${PROTOBUF_PROTOC_EXECUTABLE} ${ARGS} ${PROTO_FILE}")

EXEC_PROGRAM(${PROTOBUF_COMMAND})
