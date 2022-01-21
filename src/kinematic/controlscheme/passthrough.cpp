#include "passthrough.h"

#include <cmath>
#include <boost/log/trivial.hpp>

#include <robotcontext.h>
#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>


namespace Robot::Kinematic {


ControlSchemePassthrough::ControlSchemePassthrough(std::shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

ControlSchemePassthrough::~ControlSchemePassthrough() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemePassthrough::init() 
{
    const guard lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    
    #if 0
    m_rc_connection = m_rc_receiver->sigData.connect(
        [self_ptr=weak_from_this()] (const auto flags, const auto rssi, const auto &channels) {
            if (auto self = self_ptr.lock()) { 
                self->onRCData(flags, rssi, channels); 
            }
        }
    );
    #endif

    m_initialized = true;
}


void ControlSchemePassthrough::cleanup() 
{
    const guard lock(m_mutex);

    if (!m_initialized) 
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    for (auto &motor : m_motor_control->getMotors()) {
        motor->setEnabled(false);
        motor->servo()->setEnabled(false);
    }
}


};