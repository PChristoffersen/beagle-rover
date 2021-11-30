#ifndef _KINEMATICTYPES_H_
#define _KINEMATICTYPES_H_

#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <boost/signals2.hpp>

#include "../input/inputvalue.h"
#include "../motor/motortypes.h"

namespace Robot::Kinematic {

    using Value = ::Robot::Input::Value;

    enum class SteeringMode {
        NONE,  // No servo steering
        FRONT, // Front wheel steering
        REAR,  // Rear wheel steering
        ALL,   // 4 wheel steering
        SKID   // Skid steering
    };

    enum class DriveMode {
        NONE,
        STANDARD,
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


    constexpr double WHEEL_BASE { 178.0 };
    constexpr double WHEEL_BASE_2 { WHEEL_BASE/2.0 };
    constexpr double WHEEL_MIN_ANGLE { ::Robot::Motor::SERVO_LIMIT_MIN.asAngleRadians() };
    constexpr double WHEEL_MAX_ANGLE { ::Robot::Motor::SERVO_LIMIT_MAX.asAngleRadians() };
    constexpr double WHEEL_CENTER_ANGLE { Value::CENTER.asAngleRadians() };
    constexpr double WHEEL_STRAIGHT_ANGLE { M_PI_4 };
    constexpr double WHEEL_MAX_TURN_ANGLE { std::min(-(WHEEL_STRAIGHT_ANGLE+WHEEL_MIN_ANGLE), WHEEL_MAX_ANGLE-WHEEL_STRAIGHT_ANGLE) };


};

#endif
