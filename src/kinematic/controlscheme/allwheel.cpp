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


void ControlSchemeAllWheel::setMotors(float left, float right) 
{
    motorServo(FRONT_LEFT,  Value::fromAngle(WHEEL_STRAIGHT_ANGLE+left));
    motorServo(FRONT_RIGHT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE+right));
    motorServo(REAR_LEFT,   Value::fromAngle(WHEEL_STRAIGHT_ANGLE+left));
    motorServo(REAR_RIGHT,  Value::fromAngle(WHEEL_STRAIGHT_ANGLE+right));
}


void ControlSchemeAllWheel::setMotorDuty(float steering, float throttle, float outer_dist, float inner_dist, float inner_angle)
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