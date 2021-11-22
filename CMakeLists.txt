cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")


project(robot-control VERSION 1.0)

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs

find_package(Python3 3.7 COMPONENTS Development)
find_package(PythonLibs 3.7 REQUIRED)
find_package(Boost 1.67.0 COMPONENTS system log python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR})
find_library(ROBOTCONTROL robotcontrol)
add_definitions(-DBOOST_LOG_DYN_LINK)


# -----------------------------------------------
# Helper libraries
# -----------------------------------------------
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/pid")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/robotcontrolext")


# -----------------------------------------------
# Main src
# -----------------------------------------------
set(SRC 
    src/robot.cpp
    src/robotcontext.cpp
    src/debug/prudebug.cpp
    src/rc/rcreceiver.cpp
    src/motor/motor.cpp
    src/motor/motorgimbal.cpp
    src/motor/motorcontrol.cpp
    src/led/ledcontrol.cpp
    src/led/ledcolor.cpp
    src/led/ledcolorlayer.cpp
    src/led/animation/indicator.cpp
    src/led/animation/headlights.cpp
    src/led/animation/construction.cpp
    src/led/animation/police.cpp
    src/led/animation/ambulance.cpp
    src/led/animation/knightrider.cpp
    src/led/animation/rainbow.cpp
    src/telemetry/telemetry.cpp
    src/telemetry/sources/adcbattery.cpp
    src/telemetry/sources/rcmpu.cpp
    src/kinematic/kinematic.cpp
    src/kinematic/controlscheme/idle.cpp
    src/kinematic/controlscheme/normal.cpp
    src/kinematic/controlscheme/spinning.cpp
    src/kinematic/controlscheme/balancing.cpp
    src/kinematic/controlscheme/passthrough.cpp
    src/system/wifi.cpp
)
#target_link_directories(robot PRIVATE robotcontrol-ext)


# -----------------------------------------------
# Python module
# -----------------------------------------------
# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")
add_library(robotcontrol MODULE 
    ${SRC}
    src/python/module.cpp
    src/python/util.cpp
    src/python/export_std.cpp
    src/python/export_motor.cpp
    src/python/export_telemetry.cpp
    src/python/export_rcreceiver.cpp
    src/python/export_led.cpp
    src/python/export_kinematic.cpp
    src/python/export_robot.cpp
)
target_include_directories(robotcontrol PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(robotcontrol PRIVATE ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
target_link_libraries(robotcontrol PRIVATE pid)
target_link_libraries(robotcontrol PRIVATE robotcontrolext)

# -----------------------------------------------
# Standalone executable
# -----------------------------------------------
add_executable(test_robot
    ${SRC}
    main.cpp
)
target_include_directories(test_robot PRIVATE src)
target_link_libraries(test_robot "${Boost_LIBRARIES}")
target_link_libraries(test_robot pid)
target_link_libraries(test_robot robotcontrolext)

