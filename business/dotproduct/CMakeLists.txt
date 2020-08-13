cmake_minimum_required(VERSION 3.1)

#set the project name
project(dotproduct)

add_library(business business_v2.hpp)

target_include_directories(business PUBLIC ../../aux/)
target_compile_features(business PUBLIC cxx_std_17)

set_target_properties(business PROPERTIES LINKER_LANGUAGE CXX)

set(CMAKE_LANGUAGE_COMPILER CXX)
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS_DEBUG "-std=c++17 -g -O0 -Wall -pedantic -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++17 -O3 -Wall -pthread")

include_directories(/home/tommaso/fastflow)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../..)
#add_subdirectory(${FF_HOME})

#add the executable
add_executable(dotproduct ff_code.cpp)
#target_include_directories(dotproduct PUBLIC ${FF_HOME})
#add_library(business.hpp)
#target_link_libraries(dotproduct PUBLIC business)

# target_include_directories(./
#                           ../
#                           ${FF_HOME})
