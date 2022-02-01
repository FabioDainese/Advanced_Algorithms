cmake_minimum_required(VERSION 3.6)
project(TensorLibrary)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp NKTensor.h Utility.h Utility.cpp NKTester.h NKIterator.h NKIteratorOneDim.h ROTensor.h ROTester.h ROIterator.h ROIteratorOneDim.h)
add_executable(TensorLibrary ${SOURCE_FILES})