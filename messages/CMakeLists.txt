cmake_minimum_required(VERSION 2.8)

## generate protobuf (the default cmake command does not work well with subdirectories) ##
set(OUT_PYTHON ${CMAKE_CURRENT_SOURCE_DIR}/../python_scripts/)
set(OUT_CPP ${CMAKE_CURRENT_BINARY_DIR}/)

get_filename_component(FIL_WE ${PROTO_FILE} NAME_WE)

set(ARGS "-I=${CMAKE_CURRENT_SOURCE_DIR} --python_out ${OUT_PYTHON} --cpp_out ${OUT_CPP} --proto_path ${PROTOBUF_INCLUDE_DIRS}")
set(PROTOBUF_COMMAND "${PROTOBUF_PROTOC_EXECUTABLE} ${ARGS} ${PROTO_FILE}")
EXEC_PROGRAM(${PROTOBUF_COMMAND})


set(PROTO_SRCS ${OUT_CPP}${FIL_WE}.pb.cc PARENT_SCOPE)
set(PROTO_HDRS ${OUT_CPP}${FIL_WE}.pb.h PARENT_SCOPE)
