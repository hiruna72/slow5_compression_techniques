cmake_minimum_required(VERSION 3.11)
project(slow5_entropy)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 11)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/slow5lib/include)

add_executable(slow5_entropy slow5_entropy.cpp)

add_subdirectory(${PROJECT_SOURCE_DIR}/slow5lib)

target_link_libraries(slow5_entropy slow5 streamvbyte -lz -ldl -lm -lpthread -lrt)