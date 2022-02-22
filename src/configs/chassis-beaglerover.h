#ifndef _ROBOT_CHASSIS_BEAGLEROVER_H_
#define _ROBOT_CHASSIS_BEAGLEROVER_H_

#include <cmath>
#include <algorithm>

#include <config.h>

namespace Robot::Config {

    // Servo motor limits
    inline constexpr auto SERVO_LIMIT_MIN { 500u };
    inline constexpr auto SERVO_LIMIT_MAX { 2500u };
    inline constexpr auto SERVO_CENTER    { (SERVO_LIMIT_MAX+SERVO_LIMIT_MIN)/2u };
    inline constexpr auto SERVO_RANGE     { SERVO_LIMIT_MAX-SERVO_LIMIT_MIN };

    // Chassis dimensions
    inline constexpr float WHEEL_BASE_MM { 178.0f };
    inline constexpr float WHEEL_BASE_2_MM { WHEEL_BASE_MM/2.0f };

    // Wheel dimentions
    inline constexpr int32_t WHEEL_CIRC_MM { 300 };
    inline constexpr float WHEEL_DIAMETER_MM { WHEEL_CIRC_MM / M_PI };

    // Wheel servo limits 
    inline constexpr auto WHEEL_SERVO_LIMIT_MIN { 650u };
    inline constexpr auto WHEEL_SERVO_LIMIT_MAX { 2350u };
    inline constexpr auto WHEEL_MIN_ANGLE { (float)M_PI * (float)((int)WHEEL_SERVO_LIMIT_MIN-SERVO_CENTER)/SERVO_RANGE };
    inline constexpr auto WHEEL_MAX_ANGLE { (float)M_PI * (float)((int)WHEEL_SERVO_LIMIT_MAX-SERVO_CENTER)/SERVO_RANGE };
    inline constexpr auto WHEEL_CENTER_ANGLE { 0.0f };
    inline constexpr auto WHEEL_STRAIGHT_ANGLE { (float)M_PI_4 };
    inline constexpr auto WHEEL_MAX_TURN_ANGLE { std::min(-(WHEEL_STRAIGHT_ANGLE+WHEEL_MIN_ANGLE), WHEEL_MAX_ANGLE-WHEEL_STRAIGHT_ANGLE) };

};

#endif
