#ifndef _MOTORTYPES_H_
#define _MOTORTYPES_H_

#include "../input/inputvalue.h"

namespace Robot::Motor {

    using Value = ::Robot::Input::Value;

    constexpr Value SERVO_LIMIT_MIN { 650u };
    constexpr Value SERVO_LIMIT_MAX { 2350u };
};

#endif