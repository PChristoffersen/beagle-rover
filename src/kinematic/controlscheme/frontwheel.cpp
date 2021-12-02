#include "frontwheel.h"

#include <cmath>
#include <algorithm>
#include <boost/log/trivial.hpp>

#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"

using namespace std;

namespace Robot::Kinematic {



ControlSchemeFrontWheel::ControlSchemeFrontWheel(shared_ptr<Kinematic> kinematic) :
    AbstractWheelSteering { kinematic, WHEEL_BASE_2 }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

ControlSchemeFrontWheel::~ControlSchemeFrontWheel() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemeFrontWheel::setMotors(Value left, Value right) 
{
    const auto &motors = m_motor_control->getMotors();
    motorServo(FRONT_LEFT, left);
    motorServo(FRONT_RIGHT, right);
}


};