#include "allwheel.h"

#include <cmath>
#include <algorithm>
#include <boost/log/trivial.hpp>

#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"

using namespace std;

namespace Robot::Kinematic {



ControlSchemeAllWheel::ControlSchemeAllWheel(shared_ptr<Kinematic> kinematic) :
    AbstractWheelSteering { kinematic, WHEEL_BASE }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

ControlSchemeAllWheel::~ControlSchemeAllWheel() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemeAllWheel::setMotors(Value left, Value right) 
{
    const auto &motors = m_motor_control->getMotors();
    motorServo(FRONT_LEFT, left);
    motorServo(FRONT_RIGHT, right);
    motorServo(REAR_LEFT, -left);
    motorServo(REAR_RIGHT, -right);
}


};