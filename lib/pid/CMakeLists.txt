cmake_minimum_required(VERSION 3.10)

add_library(pid
    PID.cpp
)
target_include_directories(pid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
