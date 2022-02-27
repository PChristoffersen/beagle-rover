#include "rearwheel.h"

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

ControlSchemeRearWheel::ControlSchemeRearWheel(std::shared_ptr<Kinematic> kinematic) :
    AbstractWheelSteering { kinematic, WHEEL_BASE_2_MM }
{
}

ControlSchemeRearWheel::~ControlSchemeRearWheel() 
{
    cleanup();
}


void ControlSchemeRearWheel::setMotors(float left, float right) 
{
    motorServo(REAR_LEFT,  Value::fromAngle(WHEEL_STRAIGHT_ANGLE+left));
    motorServo(REAR_RIGHT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE+right));
    motorServo(FRONT_LEFT,  Value::fromAngle(WHEEL_STRAIGHT_ANGLE));
    motorServo(FRONT_RIGHT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE));
}


void ControlSchemeRearWheel::setMotorDuty(float steering, float throttle, float outer_dist, float inner_dist, float inner_angle)
{
    auto factor = inner_dist/outer_dist;
    if (steering>=0.0f) {
        motorDuty(FRONT_LEFT, throttle);
        motorDuty(FRONT_RIGHT, throttle*factor);
        motorDuty(REAR_LEFT, throttle);
        motorDuty(REAR_RIGHT, throttle*factor);
    }
    else {
        motorDuty(FRONT_LEFT, throttle*factor);
        motorDuty(FRONT_RIGHT, throttle);
        motorDuty(REAR_LEFT, throttle*factor);
        motorDuty(REAR_RIGHT, throttle);
    }
}


}