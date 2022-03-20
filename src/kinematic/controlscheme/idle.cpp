#include "idle.h"

#include <boost/log/trivial.hpp>
#include <boost/system/error_code.hpp>

#include <robotcontext.h>
#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"

using namespace std::literals;

namespace Robot::Kinematic {

static constexpr auto IDLE_DELAY { 250ms };


ControlSchemeIdle::ControlSchemeIdle(std::shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic }, 
    m_timer { m_context->io() }
{
}


ControlSchemeIdle::~ControlSchemeIdle() 
{
    cleanup();
}


void ControlSchemeIdle::init() 
{
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
    m_timer.async_wait(boost::asio::bind_executor(m_strand, 
        [this] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            timer(); 
        }
    ));

    m_initialized = true;
}


void ControlSchemeIdle::cleanup() 
{
    if (!m_initialized) 
        return;
    m_initialized = false;

    m_timer.cancel();
}



void ControlSchemeIdle::timer() 
{
    if (!m_initialized)
        return;

    // Disable motor and servos
    for (auto &motor : m_motor_control->getMotors()) {
        motor->servo()->setEnabled(false);
    }
}


}