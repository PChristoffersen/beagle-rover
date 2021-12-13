#ifndef _ROBOT_MOTOR_TYPES_H_
#define _ROBOT_MOTOR_TYPES_H_

#include <chrono>
#include <input/value.h>

namespace Robot::Motor {

    using Value = ::Robot::Input::Value;

    static constexpr auto MOTOR_COUNT { 4 };

    using MotorList = std::array<std::unique_ptr<class Motor>, MOTOR_COUNT>;

    static constexpr Value SERVO_LIMIT_MIN { 650u };
    static constexpr Value SERVO_LIMIT_MAX { 2350u };


    using clock_type = std::chrono::high_resolution_clock;
    static constexpr auto MOTOR_TIMER_INTERVAL { std::chrono::milliseconds(50) };
    static constexpr auto SERVO_TIMER_INTERVAL { std::chrono::milliseconds(20) };
};

#endif
