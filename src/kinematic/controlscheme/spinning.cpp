#include "spinning.h"

#include <boost/log/trivial.hpp>

#include "../kinematictypes.h"
#include "../../motor/motor.h"
#include "../../motor/motorservo.h"
#include "../../motor/motorcontrol.h"

using namespace std;

namespace Robot::Kinematic {

ControlSchemeSpinning::ControlSchemeSpinning(shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

ControlSchemeSpinning::~ControlSchemeSpinning() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemeSpinning::init() 
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    // Set all motor angles to 0 degrees and throttle to 0
    for (auto &motor : m_motor_control->getMotors()) {
        motor->setDuty(0.0);
        motor->freeSpin();
        motor->setEnabled(true);
        motor->servo()->setValue(Value::CENTER);
        motor->servo()->setEnabled(true);
    }

    m_initialized = true;
}


void ControlSchemeSpinning::cleanup() 
{
    const lock_guard<mutex> lock(m_mutex);

    if (!m_initialized) 
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}

};