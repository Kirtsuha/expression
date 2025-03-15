cmake_minimum_required(VERSION 3.29)
project(expression)

set(CMAKE_CXX_STANDARD 20)

add_library(expression SHARED library.cpp)

add_executable(check library.cpp
        test.cpp)