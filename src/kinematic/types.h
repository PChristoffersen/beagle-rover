#ifndef _ROBOT_KINEMATIC_TYPES_H_
#define _ROBOT_KINEMATIC_TYPES_H_

#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <array>
#include <boost/signals2.hpp>

#include <input/value.h>
#include <motor/types.h>

namespace Robot::Kinematic {

    using Value = ::Robot::Input::Value;

    enum class DriveMode {
        NONE,
        ALL_WHEEL,
        FRONT_WHEEL,
        REAR_WHEEL,
        SKID,
        SPINNING,
        BALANCING,
        PASSTHROUGH // Direct input from remote
    };

    enum class Orientation {
        NORTH,
        SOUTH,
        EAST,
        WEST
    };

    enum MotorPosition {
        FRONT_LEFT = 0,
        FRONT_RIGHT = 1,
        REAR_LEFT = 2,
        REAR_RIGHT = 3,
    };

    struct MotorMapEntry {
        size_t index;
        bool invert;
    };

    using MotorMap = std::array<MotorMapEntry, ::Robot::Motor::MOTOR_COUNT>;

    constexpr MotorMap MOTOR_MAP_NORTH {{
        { 0, false }, 
        { 1, false }, 
        { 2, false }, 
        { 3, false } 
    }};
    constexpr MotorMap MOTOR_MAP_SOUTH {{
        { 3, false }, 
        { 2, false }, 
        { 1, false }, 
        { 0, false } 
    }};
    constexpr MotorMap MOTOR_MAP_EAST {{ 
        { 1, false }, 
        { 3, false }, 
        { 0, false }, 
        { 2, false } 
    }};
    constexpr MotorMap MOTOR_MAP_WEST {{
        { 2, false }, 
        { 0, false }, 
        { 3, false }, 
        { 1, false } 
    }};

    constexpr double WHEEL_BASE { 178.0 };
    constexpr double WHEEL_BASE_2 { WHEEL_BASE/2.0 };
    constexpr double WHEEL_MIN_ANGLE { ::Robot::Motor::SERVO_LIMIT_MIN.asAngleRadians() };
    constexpr double WHEEL_MAX_ANGLE { ::Robot::Motor::SERVO_LIMIT_MAX.asAngleRadians() };
    constexpr double WHEEL_CENTER_ANGLE { Value::CENTER.asAngleRadians() };
    constexpr double WHEEL_STRAIGHT_ANGLE { M_PI_4 };
    constexpr double WHEEL_MAX_TURN_ANGLE { std::min(-(WHEEL_STRAIGHT_ANGLE+WHEEL_MIN_ANGLE), WHEEL_MAX_ANGLE-WHEEL_STRAIGHT_ANGLE) };


};

#endif
