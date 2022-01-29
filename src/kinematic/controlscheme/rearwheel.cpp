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
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

ControlSchemeRearWheel::~ControlSchemeRearWheel() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemeRearWheel::setMotors(float left, float right) 
{
    motorServo(REAR_LEFT, -Value::fromAngle(WHEEL_STRAIGHT_ANGLE+left));
    motorServo(REAR_RIGHT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE+right));
}


};