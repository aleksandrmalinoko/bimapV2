cmake_minimum_required(VERSION 3.15)
project(bimap)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(lib/googletest-master)
include_directories(lib/googletest-master/googletest/include)

add_executable(bimap main.cpp Tests/bimap.h Tests/main.cpp)
target_link_libraries(bimap gtest gtest_main)
