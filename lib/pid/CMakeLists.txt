cmake_minimum_required(VERSION 3.10)

add_library(pid STATIC
    PID.cpp
)
set_property(TARGET pid PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(pid PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
