cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")

project(beaglerover VERSION 1.0)
enable_testing()

set(Boost_USE_STATIC_LIBS       OFF)
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)
add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-unused-parameter)
add_compile_options("$<$<CONFIG:DEBUG>:-DROBOT_DEBUG>")

find_package(Threads REQUIRED)
find_package(Python3 3.7 COMPONENTS Development)
find_package(PythonLibs 3.7 REQUIRED)
find_package(Boost 1.67.0 COMPONENTS system log numpy${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR} python${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR})
add_definitions(-DBOOST_LOG_DYN_LINK)

# -----------------------------------------------
# Helper libraries
# -----------------------------------------------
find_library(ROBOTCONTROL robotcontrol)
if(ROBOTCONTROL)
    set(HAVE_ROBOTCONTROL 1)
    add_subdirectory("${CMAKE_SOURCE_DIR}/lib/robotcontrolext")
    include_directories("${CMAKE_SOURCE_DIR}/lib/robotcontrolext")
endif()


message("Searching for Linux netlink library")
find_path(LibNL_INCLUDE_DIR netlink/netlink.h
	/usr/include
	/usr/include/libnl3
	/usr/local/include
	/usr/local/include/libnl3
)
find_library(LibNL_LIBRARY NAMES nl nl-3)
find_library(LibNL_ROUTE_LIBRARY NAMES nl-route nl-route-3)
#find_library(LibNL_NETFILTER_LIBRARY NAMES nl-nf nl-nf-3)
find_library(LibNL_GENL_LIBRARY NAMES nl-genl nl-genl-3)

if (LibNL_INCLUDE_DIR AND LibNL_LIBRARY)
	set(LibNL_FOUND TRUE)
endif (LibNL_INCLUDE_DIR AND LibNL_LIBRARY)

if (LibNL_FOUND)
    #set(LibNL_LIBRARIES ${LibNL_LIBRARY} ${LibNL_ROUTE_LIBRARY} ${LibNL_NETFILTER_LIBRARY} ${LibNL_GENL_LIBRARY})
    set(LibNL_LIBRARIES ${LibNL_LIBRARY} ${LibNL_ROUTE_LIBRARY} ${LibNL_GENL_LIBRARY})
    message("Found netlink libraries:  ${LibNL_LIBRARIES}")
    message("Found netlink includes: ${LibNL_INCLUDE_DIR}")
    include_directories("${LibNL_INCLUDE_DIR}")
ELSE (LibNL_FOUND)
    message("Netlink version 3 development packages cannot be found.")
    message("In Debian/Ubuntu, they may be called:")
    message("libnl-3-dev libnl-genl-3dev libnl-nf-3-dev libnl-route-3-dev")
    message(FATAL_ERROR "Could not find netlink library.")
endif (LibNL_FOUND)



# -----------------------------------------------
# Determine git hash
# -----------------------------------------------
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_custom_target(AlwaysCheckGit COMMAND ${CMAKE_COMMAND}
    DRUN_CHECK_GIT_VERSION=1
    -Dpre_configure_dir=${pre_configure_dir}
    -Dpost_configure_file=${post_configure_dir}
    -DGIT_HASH_CACHE=${GIT_HASH_CACHE}
    -P ${CURRENT_LIST_DIR}/CheckGit.cmake
    BYPRODUCTS ${post_configure_file}
)

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
    src/robotlogging.cpp
    src/robotdebug.cpp
    src/rc/receiver.cpp
    src/rc/inputsource.cpp
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
    src/led/animation/runninglight.cpp
    src/led/animation/rainbow.cpp
    src/led/animation/rainbowwave.cpp
    src/telemetry/telemetry.cpp
    src/telemetry/events.cpp
    src/telemetry/sources/motors.cpp
    src/telemetry/sources/odometer.cpp
    src/telemetry/sources/robotcontrolbattery.cpp
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
    src/input/control.cpp
    src/input/sources/software.cpp
    src/input/sources/gamepad.cpp
    src/system/network.cpp
    src/system/power.cpp
    src/hardware/abstractpower.cpp
    src/hardware/beaglebone/servopower.cpp
    src/hardware/beaglebone/motorpower.cpp
    src/hardware/beaglebone/prudebug.cpp
    src/math/pid.cpp
)
include_directories(BEFORE SYSTEM src)


# -----------------------------------------------
# C++ Library
# -----------------------------------------------
add_library(beaglerover SHARED 
    ${SRC}
)
set_property(TARGET beaglerover PROPERTY POSITION_INDEPENDENT_CODE ON)
target_include_directories(beaglerover PRIVATE ${PYTHON_INCLUDE_DIRS} ${NL3_INCLUDE_DIRS})
target_link_libraries(beaglerover PRIVATE Threads::Threads ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY} ${LibNL_LIBRARIES})
if(ROBOTCONTROL)
target_link_libraries(beaglerover PRIVATE robotcontrolext)
endif()


# -----------------------------------------------
# Python module
# -----------------------------------------------
# Without this, any build libraries automatically have names "lib{x}.so"
if (Python3_SOABI)
set(PYTHON_MODULE_POSTFIX ${Python3_SOABI})
else()
set(PYTHON_MODULE_POSTFIX "cpython-${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}${_Python3_ABI}-${CMAKE_LIBRARY_ARCHITECTURE}")
endif()

set(CMAKE_SHARED_MODULE_PREFIX "")
add_library(beaglerover_python MODULE 
    ${SRC}
    src/python/module.cpp
    src/python/util.cpp
    src/python/export_motor.cpp
    src/python/export_telemetry.cpp
    src/python/export_rcreceiver.cpp
    src/python/export_led.cpp
    src/python/export_kinematic.cpp
    src/python/export_input.cpp
    src/python/export_system.cpp
    src/python/export_robot.cpp
    src/python/export_misc.cpp
)
set_property(TARGET beaglerover_python PROPERTY POSITION_INDEPENDENT_CODE ON)
set_target_properties(beaglerover_python PROPERTIES OUTPUT_NAME "beaglerover.${PYTHON_MODULE_POSTFIX}")
target_include_directories(beaglerover_python PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(beaglerover_python PRIVATE Threads::Threads ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${Boost_NUMPY_LIBRARY} ${LibNL_LIBRARIES})
if(ROBOTCONTROL)
target_link_libraries(beaglerover_python PRIVATE robotcontrolext)
endif()




# -----------------------------------------------
# Standalone executable
# -----------------------------------------------
add_executable(example_robot examples/example_robot.cpp )
target_include_directories(example_robot PRIVATE src)
target_link_libraries(example_robot beaglerover Threads::Threads ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY})



# -----------------------------------------------
# Tests
# -----------------------------------------------
add_executable(test_robot test/test_robot.cpp )
target_include_directories(test_robot PRIVATE src)
target_link_libraries(test_robot beaglerover Threads::Threads ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY})
add_test(NAME Robot COMMAND test_robot)
 
add_executable(test_leds test/test_leds.cpp )
target_include_directories(test_leds PRIVATE src)
target_link_libraries(test_leds beaglerover Threads::Threads ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY})
add_test(NAME LEDS COMMAND test_leds)

add_executable(test_color test/test_color.cpp )
target_include_directories(test_color PRIVATE src)
target_link_libraries(test_color beaglerover Threads::Threads ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY})
add_test(NAME Color COMMAND test_color)

add_executable(test_value test/test_value.cpp )
target_include_directories(test_value PRIVATE src)
target_link_libraries(test_value beaglerover Threads::Threads ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY})
add_test(NAME Value COMMAND test_value)

add_executable(test_subscription test/test_subscription.cpp )
target_include_directories(test_subscription PRIVATE src)
target_link_libraries(test_subscription beaglerover Threads::Threads ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY})
add_test(NAME Subscription COMMAND test_subscription)

add_executable(test_network test/test_network.cpp )
target_include_directories(test_network PRIVATE src)
target_link_libraries(test_network beaglerover Threads::Threads ${Boost_SYSTEM_LIBRARY} ${Boost_LOG_LIBRARY})
add_test(NAME Network COMMAND test_network)

# Python tests
add_test(NAME PythonSubscription COMMAND test_subscription.py WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
add_test(NAME PythonColors COMMAND test_color.py WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)
add_test(NAME PythonLEDS COMMAND test_leds.py WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test)



# Print cmake variables
#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
