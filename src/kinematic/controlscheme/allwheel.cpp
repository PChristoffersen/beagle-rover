#include "allwheel.h"

#include <cmath>
#include <algorithm>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"

using namespace Robot::Config;

namespace Robot::Kinematic {



ControlSchemeAllWheel::ControlSchemeAllWheel(std::shared_ptr<Kinematic> kinematic) :
    AbstractWheelSteering { kinematic, WHEEL_BASE_MM }
{
}

ControlSchemeAllWheel::~ControlSchemeAllWheel() 
{
    cleanup();
}


void ControlSchemeAllWheel::setMotors(float left, float right, float skew) 
{
    motorServo(MotorPosition::FRONT_LEFT,  Value::fromAngle(WHEEL_STRAIGHT_ANGLE+left-skew));
    motorServo(MotorPosition::FRONT_RIGHT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE+right+skew));
    motorServo(MotorPosition::REAR_LEFT,   Value::fromAngle(WHEEL_STRAIGHT_ANGLE+left+skew));
    motorServo(MotorPosition::REAR_RIGHT,  Value::fromAngle(WHEEL_STRAIGHT_ANGLE+right-skew));
}


void ControlSchemeAllWheel::setMotorDuty(float steering, float throttle, float outer_dist, float inner_dist, float inner_angle)
{
    auto factor = inner_dist/outer_dist;
    if (steering>=0.0f) {
        motorDuty(MotorPosition::FRONT_LEFT, throttle);
        motorDuty(MotorPosition::FRONT_RIGHT, throttle*factor);
        motorDuty(MotorPosition::REAR_LEFT, throttle);
        motorDuty(MotorPosition::REAR_RIGHT, throttle*factor);
    }
    else {
        motorDuty(MotorPosition::FRONT_LEFT, throttle*factor);
        motorDuty(MotorPosition::FRONT_RIGHT, throttle);
        motorDuty(MotorPosition::REAR_LEFT, throttle*factor);
        motorDuty(MotorPosition::REAR_RIGHT, throttle);
    }
}


}