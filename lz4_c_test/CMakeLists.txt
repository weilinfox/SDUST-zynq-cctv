cmake_minimum_required(VERSION 3.22)
project(lz4_c_test C)

set(CMAKE_C_STANDARD 99)

add_executable(lz4_c_test main.c lz4-1.9.4/lz4.c)
