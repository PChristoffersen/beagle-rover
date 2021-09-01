cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")

project(beagle-rover-1 VERSION 1.0)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_DEBUG_LIBS        OFF)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
find_package(Boost 1.67.0 REQUIRED COMPONENTS system thread)
IF(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

find_library(ROBOTCONTROL robotcontrol)

add_subdirectory("${CMAKE_SOURCE_DIR}/robotcontrol-ext")

add_library(robot
    component.cpp
    robot.cpp
    motorcontrol.cpp
    battery.cpp
)
target_link_directories(robot PRIVATE robotcontrol-ext)
target_link_libraries(robot PRIVATE robotcontrol-ext)
target_link_libraries(robot PRIVATE "${ROBOTCONTROL}")
target_link_libraries(robot PRIVATE "${Boost_LIBRARIES}")


add_executable(test_robot
    main.cpp
)
target_link_libraries(test_robot PRIVATE robot)
target_link_libraries(test_robot PRIVATE robotcontrol-ext)
target_link_libraries(test_robot PRIVATE "${Boost_LIBRARIES}")
