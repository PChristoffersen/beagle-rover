#include "controlschemenormal.h"

#include <math.h>
#include <boost/log/trivial.hpp>

#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"

using namespace std;


ControlSchemeNormal::ControlSchemeNormal(shared_ptr<class Kinematic> kinematic) :
    AbstractControlScheme { kinematic }
{
    BOOST_LOG_TRIVIAL(trace) << "Normal()";
}

ControlSchemeNormal::~ControlSchemeNormal() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << "~Normal()";
}


void ControlSchemeNormal::init() 
{
    const lock_guard<mutex> lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    
    for (auto &motor : m_motor_control->getMotors()) {
        motor->setDuty(0.0);
        motor->setEnabled(true);
        motor->gimbal()->setAngle(M_PI_2);
        motor->gimbal()->setEnabled(true);
    }

    m_initialized = true;
}


void ControlSchemeNormal::cleanup() 
{
    const lock_guard<mutex> lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}
