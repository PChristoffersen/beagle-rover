cmake_minimum_required(VERSION 3.10)

find_library(ROBOTCONTROL robotcontrol)

add_library(robotcontrolext STATIC
    rc_ext_debug.c
    rc_ext_pru.c
    rc_ext_encoder.c
    rc_ext_fbus.c
    rc_ext_neopixel.c
)
set_property(TARGET robotcontrolext PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(robotcontrolext PUBLIC "${ROBOTCONTROL}")
target_include_directories(robotcontrolext PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
