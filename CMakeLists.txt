cmake_minimum_required(VERSION 3.27)
project(lexical)

set(CMAKE_CXX_STANDARD 20)

add_executable(lexical lexical_util.cpp lexical_analyser1.cpp main.cpp cpp_feature.cpp cpp_feature.h)
