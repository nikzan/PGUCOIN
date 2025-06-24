cmake_minimum_required(VERSION 3.10)
project(PGUCOIN CXX)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(PGUCOIN ${SOURCES})

file(COPY data/ DESTINATION ${CMAKE_BINARY_DIR}/data)