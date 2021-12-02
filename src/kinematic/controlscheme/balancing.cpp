#include "balancing.h"

#include <cmath>
#include <boost/log/trivial.hpp>

#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"

using namespace std;

namespace Robot::Kinematic {


ControlSchemeBalancing::ControlSchemeBalancing(shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


ControlSchemeBalancing::~ControlSchemeBalancing() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemeBalancing::init() 
{
    const guard lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    auto &motors = m_motor_control->getMotors();

    // Set all motor angles to 0 degrees and throttle to 0
    {
        auto &motor = motors[MotorPosition::FRONT_LEFT];
        motor->setDuty(0.0);
        motor->brake();
        motor->setEnabled(true);
        motor->servo()->setValue(Value::fromAngleRadians(-M_PI_2));
        motor->servo()->setEnabled(true);
    }
    {
        auto &motor = motors[MotorPosition::FRONT_RIGHT];
        motor->setDuty(0.0);
        motor->brake();
        motor->setEnabled(true);
        motor->servo()->setValue(Value::fromAngleRadians(-M_PI_2));
        motor->servo()->setEnabled(true);
    }
    {
        auto &motor = motors[MotorPosition::REAR_LEFT];
        motor->setDuty(0.0);
        motor->setEnabled(true);
        motor->servo()->setValue(Value::fromAngleRadians(M_PI_2));
        motor->servo()->setEnabled(true);
    }
    {
        auto &motor = motors[MotorPosition::REAR_RIGHT];
        motor->setDuty(0.0);
        motor->setEnabled(true);
        motor->servo()->setValue(Value::fromAngleRadians(M_PI_2));
        motor->servo()->setEnabled(true);
    }

    m_initialized = true;
}


void ControlSchemeBalancing::cleanup() 
{
    const guard lock(m_mutex);

    if (!m_initialized) 
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


};