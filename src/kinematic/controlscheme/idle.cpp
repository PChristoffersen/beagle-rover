#include "idle.h"

#include <boost/log/trivial.hpp>
#include <boost/system/error_code.hpp>

#include <robotcontext.h>
#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"

using namespace std;

namespace Robot::Kinematic {

static constexpr auto IDLE_DELAY { 250ms };


ControlSchemeIdle::ControlSchemeIdle(shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic }, 
    m_timer { m_context->io() }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


ControlSchemeIdle::~ControlSchemeIdle() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemeIdle::init() 
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << this << ": " << __FUNCTION__;

    // Set all motor angles to 0 degrees and throttle to 0
    for (auto &motor : m_motor_control->getMotors()) {
        motor->setDuty(0.0);
        motor->freeSpin();
        motor->setEnabled(false);
        motor->servo()->setValue(Value::CENTER);
        motor->servo()->setEnabled(true);
    }

    // Start timer to turn off power to the motors
    m_timer.expires_after(IDLE_DELAY);
    m_timer.async_wait(
        [self_ptr=weak_from_this()] (auto &error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            if (auto self = self_ptr.lock()) { 
                self->timer(); 
            }
        }
    );


    m_initialized = true;
}


void ControlSchemeIdle::cleanup() 
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(trace) << this << ": " << __FUNCTION__;

    m_timer.cancel();

    BOOST_LOG_TRIVIAL(info) << this << ": " << __FUNCTION__ << "  <<<<";
}



void ControlSchemeIdle::timer() 
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;

    BOOST_LOG_TRIVIAL(info) << this << ": " << __FUNCTION__;

    // Disable motor and servos
    for (auto &motor : m_motor_control->getMotors()) {
        motor->servo()->setEnabled(false);
    }
}


};