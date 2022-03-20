#ifndef _ROBOT_MOTOR_TYPES_H_
#define _ROBOT_MOTOR_TYPES_H_

#include <chrono>
#include <common/value.h>

namespace Robot::Motor {

    class Motor;
    class Servo;

    using Value = ::Robot::Value;

    inline constexpr auto MOTOR_COUNT { 4 };
    
    using MotorList = std::array<std::unique_ptr<Motor>, MOTOR_COUNT>;
    using ServoList = std::array<std::unique_ptr<Servo>, MOTOR_COUNT>;

    using clock_type = std::chrono::high_resolution_clock;
    inline constexpr auto MOTOR_TIMER_INTERVAL { std::chrono::milliseconds(20) };
    inline constexpr auto SERVO_TIMER_INTERVAL { std::chrono::milliseconds(20) };
}

#endif
