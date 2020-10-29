cmake_minimum_required(VERSION 3.14)
project(aux)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(aux main.cpp sequential.hpp)