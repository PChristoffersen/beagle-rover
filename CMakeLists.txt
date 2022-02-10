cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")

project(robotsystem VERSION 1.0)

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)

find_package(Python3 3.7 COMPONENTS Development)
find_package(PythonLibs 3.7 REQUIRED)
find_package(Boost 1.67.0 COMPONENTS system log python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR})
find_library(ROBOTCONTROL robotcontrol)
add_definitions(-DBOOST_LOG_DYN_LINK)


# -----------------------------------------------
# Helper libraries
# -----------------------------------------------
if(ROBOTCONTROL)
    set(HAVE_ROBOTCONTROL 1)
    add_subdirectory("${CMAKE_SOURCE_DIR}/lib/robotcontrolext")
endif()

# -----------------------------------------------
# Configuration
# -----------------------------------------------
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
)
include_directories("${PROJECT_BINARY_DIR}")


# -----------------------------------------------
# Main src
# -----------------------------------------------
set(SRC 
    src/robot.cpp
    src/robotcontext.cpp
    src/rc/receiver.cpp
    src/motor/motor.cpp
    src/motor/servo.cpp
    src/motor/control.cpp
    src/led/control.cpp
    src/led/color.cpp
    src/led/colorlayer.cpp
    src/led/animation/indicator.cpp
    src/led/animation/headlights.cpp
    src/led/animation/construction.cpp
    src/led/animation/police.cpp
    src/led/animation/ambulance.cpp
    src/led/animation/knightrider.cpp
    src/led/animation/rainbow.cpp
    src/telemetry/telemetry.cpp
    src/telemetry/events.cpp
    src/telemetry/sources/adcbattery.cpp
    src/telemetry/sources/robotcontrolmpu.cpp
    src/kinematic/kinematic.cpp
    src/kinematic/controlscheme/abstractcontrolscheme.cpp
    src/kinematic/controlscheme/abstractwheelsteering.cpp
    src/kinematic/controlscheme/idle.cpp
    src/kinematic/controlscheme/allwheel.cpp
    src/kinematic/controlscheme/frontwheel.cpp
    src/kinematic/controlscheme/rearwheel.cpp
    src/kinematic/controlscheme/skid.cpp
    src/kinematic/controlscheme/spinning.cpp
    src/kinematic/controlscheme/balancing.cpp
    src/kinematic/controlscheme/passthrough.cpp
    src/input/control.cpp
    src/input/sources/software.cpp
    src/input/sources/rc.cpp
    src/input/sources/gamepad.cpp
    src/system/prudebug.cpp
    src/system/wifi.cpp
    src/hardware/abstractpower.cpp
    src/hardware/beaglebone/servopower.cpp
    src/hardware/beaglebone/motorpower.cpp
    src/math/pid.cpp
)
include_directories(BEFORE SYSTEM src)


# -----------------------------------------------
# Python module
# -----------------------------------------------
# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")
add_library(robotsystem MODULE 
    ${SRC}
    src/python/module.cpp
    src/python/util.cpp
    src/python/subscription.cpp
    src/python/export_motor.cpp
    src/python/export_telemetry.cpp
    src/python/export_rcreceiver.cpp
    src/python/export_led.cpp
    src/python/export_kinematic.cpp
    src/python/export_input.cpp
    src/python/export_robot.cpp
    src/python/export_misc.cpp
)
set_property(TARGET robotsystem PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(robotsystem PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(robotsystem PRIVATE ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
if(ROBOTCONTROL)
target_link_libraries(robotsystem PRIVATE robotcontrolext)
endif()

# -----------------------------------------------
# Standalone executable
# -----------------------------------------------
add_executable(test_robot
    ${SRC}
    main.cpp
)
target_include_directories(test_robot PRIVATE src)
target_link_libraries(test_robot ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
if(ROBOTCONTROL)
target_link_libraries(test_robot robotcontrolext)
endif()
