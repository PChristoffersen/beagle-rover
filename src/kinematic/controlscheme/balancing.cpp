#include "balancing.h"

#include <cmath>
#include <boost/log/trivial.hpp>

#include "../../motor/motor.h"
#include "../../motor/motorgimbal.h"
#include "../../motor/motorcontrol.h"

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
    const lock_guard<mutex> lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    auto &motors = m_motor_control->getMotors();

    // Set all motor angles to 0 degrees and throttle to 0
    {
        auto &motor = motors[Robot::Motor::Control::FRONT_LEFT];
        motor->setDuty(0.0);
        motor->brake();
        motor->setEnabled(true);
        motor->gimbal()->setAngle(-M_PI_2);
        motor->gimbal()->setEnabled(true);
    }
    {
        auto &motor = motors[Robot::Motor::Control::FRONT_RIGHT];
        motor->setDuty(0.0);
        motor->brake();
        motor->setEnabled(true);
        motor->gimbal()->setAngle(-M_PI_2);
        motor->gimbal()->setEnabled(true);
    }
    {
        auto &motor = motors[Robot::Motor::Control::REAR_LEFT];
        motor->setDuty(0.0);
        motor->setEnabled(true);
        motor->gimbal()->setAngle(M_PI_2);
        motor->gimbal()->setEnabled(true);
    }
    {
        auto &motor = motors[Robot::Motor::Control::REAR_RIGHT];
        motor->setDuty(0.0);
        motor->setEnabled(true);
        motor->gimbal()->setAngle(M_PI_2);
        motor->gimbal()->setEnabled(true);
    }

    m_initialized = true;
}


void ControlSchemeBalancing::cleanup() 
{
    const lock_guard<mutex> lock(m_mutex);

    if (!m_initialized) 
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


};