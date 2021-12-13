#include "rearwheel.h"

#include <cmath>
#include <algorithm>
#include <boost/log/trivial.hpp>

#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"


namespace Robot::Kinematic {



ControlSchemeRearWheel::ControlSchemeRearWheel(std::shared_ptr<Kinematic> kinematic) :
    AbstractWheelSteering { kinematic, WHEEL_BASE_2 }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

ControlSchemeRearWheel::~ControlSchemeRearWheel() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemeRearWheel::setMotors(Value left, Value right) 
{
    const auto &motors = m_motor_control->getMotors();
    motorServo(REAR_LEFT, left);
    motorServo(REAR_RIGHT, right);
}


};