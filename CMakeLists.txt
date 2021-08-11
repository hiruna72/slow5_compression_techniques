cmake_minimum_required(VERSION 3.17)
project(slow5_compression_techniques)

set(CMAKE_CXX_STANDARD 11)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/slow5lib/include)

set(vbz "${PROJECT_SOURCE_DIR}/vbz/libvbz.a")
set(streambyte "${PROJECT_SOURCE_DIR}/streamvbyte_lib/lib/libstreamvbyte_static.a")

add_executable(slow5_compression_techniques main.cpp)

add_subdirectory(${PROJECT_SOURCE_DIR}/slow5lib)

target_link_libraries(slow5_compression_techniques slow5 ${vbz} ${streambyte} -lzstd -lz -ldl -lm -lpthread -lrt)