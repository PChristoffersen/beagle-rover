#include "frontwheel.h"

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


ControlSchemeFrontWheel::ControlSchemeFrontWheel(std::shared_ptr<Kinematic> kinematic) :
    AbstractWheelSteering { kinematic, WHEEL_BASE_2_MM }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

ControlSchemeFrontWheel::~ControlSchemeFrontWheel() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemeFrontWheel::setMotors(float left, float right) 
{
    motorServo(FRONT_LEFT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE+left));
    motorServo(FRONT_RIGHT, -Value::fromAngle(WHEEL_STRAIGHT_ANGLE+right));
}


};