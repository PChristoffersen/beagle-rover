cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")



project(beagle-rover-1 VERSION 1.0)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
find_package(PythonLibs 3.7 REQUIRED)
find_package(Boost 1.67.0 REQUIRED COMPONENTS system thread python REQUIRED)
find_library(ROBOTCONTROL robotcontrol)

# -----------------------------------------------
# Helper libraries
# -----------------------------------------------
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/pid")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/robotcontrol-ext")


# -----------------------------------------------
# Main src
# -----------------------------------------------
set(SRC 
    src/component.cpp
    src/robot.cpp
    src/prudebug.cpp
    src/fbus.cpp
    src/motor/motor.cpp
    src/motor/motorcontrol.cpp
    src/led/ledcontrol.cpp
    src/led/ledanimation.cpp
    src/telemetry/telemetry.cpp
    src/telemetry/battery.cpp
)
#target_link_directories(robot PRIVATE robotcontrol-ext)


# -----------------------------------------------
# Python module
# -----------------------------------------------
# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")
add_library(beaglerover MODULE 
    ${SRC}
    src/python.cpp
)
target_include_directories(beaglerover PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(beaglerover PRIVATE ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
target_link_libraries(beaglerover PRIVATE pid)
target_link_libraries(beaglerover PRIVATE robotcontrol-ext)

# -----------------------------------------------
# Standalone executable
# -----------------------------------------------
add_executable(test_robot
    ${SRC}
    main.cpp
)
target_include_directories(test_robot PRIVATE src)
target_link_libraries(test_robot PRIVATE pid)
target_link_libraries(test_robot PRIVATE robotcontrol-ext)
target_link_libraries(test_robot PRIVATE "${Boost_LIBRARIES}")
