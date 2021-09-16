cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")

# Without this, any build libraries automatically have names "lib{x}.so"
set(CMAKE_SHARED_MODULE_PREFIX "")


project(beagle-rover-1 VERSION 1.0)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
find_package(PythonLibs 3.7 REQUIRED)
find_package(Boost 1.67.0 REQUIRED COMPONENTS system thread python REQUIRED)
find_library(ROBOTCONTROL robotcontrol)

add_subdirectory("${CMAKE_SOURCE_DIR}/lib/pid")
add_subdirectory("${CMAKE_SOURCE_DIR}/lib/robotcontrol-ext")

add_library(robot
    component.cpp
    robot.cpp
    prudebug.cpp
    fbus.cpp
    motor.cpp
    motorcontrol.cpp
    led/ledcontrol.cpp
    led/ledanimation.cpp
    telemetry/telemetry.cpp
    telemetry/battery.cpp
)

#target_link_directories(robot PRIVATE robotcontrol-ext)
target_link_libraries(robot PRIVATE pid)
target_link_libraries(robot PRIVATE robotcontrol-ext)
target_link_libraries(robot PRIVATE "${ROBOTCONTROL}")
target_link_libraries(robot PRIVATE "${Boost_LIBRARIES}")


add_executable(test_robot
    main.cpp
)
target_link_libraries(test_robot PRIVATE robot)
target_link_libraries(test_robot PRIVATE pid)
target_link_libraries(test_robot PRIVATE robotcontrol-ext)
target_link_libraries(test_robot PRIVATE "${Boost_LIBRARIES}")
