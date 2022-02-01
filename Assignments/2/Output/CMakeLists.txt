cmake_minimum_required(VERSION 3.6)
project(es2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES tensor.cc proxy.h label.h)
add_executable(es2 ${SOURCE_FILES})