#ifndef _ROBOT_KINEMATIC_TYPES_H_
#define _ROBOT_KINEMATIC_TYPES_H_

#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <array>
#include <boost/signals2.hpp>

#include <common/value.h>
#include <motor/types.h>

namespace Robot::Kinematic {

    using Value = ::Robot::Value;

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
    std::ostream &operator<<(std::ostream &os, const Orientation &orientation);


    enum MotorPosition {
        FRONT_LEFT = 0,
        FRONT_RIGHT = 1,
        REAR_LEFT = 2,
        REAR_RIGHT = 3,
    };

    struct MotorMapEntry {
        size_t index;
        bool invert;
        bool invert_duty;
    };

    using MotorMap = std::array<MotorMapEntry, ::Robot::Motor::MOTOR_COUNT>;

    constexpr MotorMap MOTOR_MAP_NORTH {{
        { 0, false, false }, 
        { 1, true,  true  }, 
        { 2, true,  false }, 
        { 3, false, true  },
    }};
    constexpr MotorMap MOTOR_MAP_SOUTH {{
        { 3, false, false }, 
        { 2, true,  true  }, 
        { 1, true,  false }, 
        { 0, false, true  } 
    }};
    constexpr MotorMap MOTOR_MAP_WEST {{
        { 2, false, false }, 
        { 0, true,  true  }, 
        { 3, true,  false }, 
        { 1, false, true  } 
    }};
    constexpr MotorMap MOTOR_MAP_EAST {{ 
        { 1, false, false }, 
        { 3, true,  true  }, 
        { 0, true,  false }, 
        { 2, false, true  } 
    }};

    class Kinematic;

}

#endif
