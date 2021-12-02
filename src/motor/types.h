#ifndef _ROBOT_MOTOR_TYPES_H_
#define _ROBOT_MOTOR_TYPES_H_

#include <input/value.h>

namespace Robot::Motor {

    using Value = ::Robot::Input::Value;

    static constexpr auto MOTOR_COUNT { 4 };

    using MotorList = std::array<std::unique_ptr<class Motor>, MOTOR_COUNT>;

    static constexpr Value SERVO_LIMIT_MIN { 650u };
    static constexpr Value SERVO_LIMIT_MAX { 2350u };
};

#endif
