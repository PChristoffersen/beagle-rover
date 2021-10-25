#include <boost/log/trivial.hpp>
#include <boost/system/error_code.hpp>

#include "controlidle.h"
#include "../robotcontext.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"


static constexpr auto IDLE_DELAY = std::chrono::seconds(2);

ControlIdle::ControlIdle(std::shared_ptr<class Kinematic> kinematic) :
    AbstractControlScheme(kinematic), 
    m_initialized(false),
    m_timer(m_context->io())
{
    BOOST_LOG_TRIVIAL(trace) << this << ": " << __FUNCTION__;
}

ControlIdle::~ControlIdle() {
    BOOST_LOG_TRIVIAL(trace) << this << ": " << __FUNCTION__;
    cleanup();
}


void ControlIdle::init() {
    const std::lock_guard<std::mutex> lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << this << ": " << __FUNCTION__;

#if 0
    // Set all motor angles to 0 degrees and throttle to 0
    for (auto &motor : m_motor_control->getMotors()) {
        motor->freeSpin();
        motor->gimbal().setAngle(0.0);
        motor->gimbal().setEnabled(true);
    }
#endif

    m_timer.expires_after(IDLE_DELAY);
    m_timer.async_wait(boost::bind(&ControlIdle::timer, shared_from_this(), _1));
    m_timer_pending = true;

    m_initialized = true;
}


void ControlIdle::cleanup() {
    const std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_initialized)
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(trace) << this << ": " << __FUNCTION__;

    if (m_timer_pending)
        m_timer.cancel();

    // Wait for cancel to take effect

    BOOST_LOG_TRIVIAL(trace) << this << ": " << __FUNCTION__ << "  <<<<";
}



void ControlIdle::timer(boost::system::error_code error) {
    BOOST_LOG_TRIVIAL(trace) << this << ": " << __FUNCTION__ << "  err="  << error;

    const std::lock_guard<std::mutex> lock(m_mutex);
    m_timer_pending = false;
    if (error!=boost::system::errc::success || !m_initialized)
        return;



    // Disable gimbals
    for (auto &motor : m_motor_control->getMotors()) {
        motor->freeSpin();
        motor->gimbal().setEnabled(false);
    }
}
