#include "motorcontrol.h"

#include <iostream>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "motor.h"
#include "motorgimbal.h"
#include "../robotcontext.h"
#include "../rc/rcreceiver.h"

using namespace std;


namespace Robot::Motor {


static constexpr auto TIMER_INTERVAL { 20ms };



Control::Control(const shared_ptr<Robot::Context> &context) : 
    m_initialized { false },
    m_enabled { false },
    m_passthrough { false },
    m_timer { context->io() }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
    case CATEGORY_PC:
        for (uint i=0; i<MOTOR_COUNT; i++) {
            m_motors[i] = make_unique<Motor>(i, m_mutex, context);
        }
        break;
    }
}


Control::~Control() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void Control::init() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    for (auto &motor : m_motors) {
       motor->init();
    }

    m_timer.expires_after(TIMER_INTERVAL);
    timer_setup();

    m_initialized = true;
}


void Control::cleanup() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    if (!m_initialized) 
        return;
    m_initialized = false;
    
    m_timer.cancel();

    setEnabled(false);

    for (auto &motor : m_motors) {
       motor->cleanup();
    }

}





void Control::brake() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    for (auto &motor : m_motors) {
       motor->brake();
    }
}

void Control::freeSpin() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    for (auto &motor : m_motors) {
       motor->freeSpin();
    }
}



void Control::setEnabled(bool enabled) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    m_enabled = enabled;

    for (auto &motor : m_motors) {
        motor->setEnabled(m_enabled);
        motor->gimbal()->setEnabled(false);
    }
}


void Control::resetOdometer() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    for (auto &motor : m_motors) {
        motor->resetOdometer();
    }
}



void Control::setPassthrough(bool passthrough, off_t servo_offset) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (passthrough != m_passthrough) {
        m_passthrough = passthrough;
        for (auto &motor : m_motors) {
            motor->setPassthrough(passthrough);
            motor->gimbal()->setPassthrough(passthrough);
        }

        // Set limits
        array<fbus_servo_limit_t, RC_SERVO_CH_MAX> limits;
        for (auto &limit : limits) {
            limit.low = Robot::InputValue::PULSE_MIN;
            limit.high = Robot::InputValue::PULSE_MAX;
        }
        for (const auto &motor : m_motors) {
            auto &limit = limits[servo_offset+motor->gimbal()->getIndex()];
            limit.low = motor->gimbal()->getLimitMin();
            limit.high = motor->gimbal()->getLimitMax();

        }
        rc_ext_fbus_set_servo_limit(limits.data());

        // Set servo map
        array<uint8_t, FBUS_CHANNELS> map;
        map.fill(FBUS_SERVO_UNMAPPED);
        if (passthrough) {
            for (const auto &motor : m_motors) {
                auto index = motor->gimbal()->getIndex();
                map[servo_offset+index] = index;
            }
        }
        rc_ext_fbus_set_servo_map(map.data());
    }
}


void Control::timer_setup() {
    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(
        [self_ptr=weak_from_this()] (auto &error) {
            if (auto self = self_ptr.lock()) { 
                self->timer(error); 
            }
        }
    );
}


void Control::timer(boost::system::error_code error) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (error!=boost::system::errc::success || !m_initialized) {
        return;
    }


    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;

    // Update control scheme

    // Set motor gimbal angle if fbus passthrough is enabled

    // Update motors
    for (auto &motor : m_motors) {
        motor->update();
    }

    timer_setup();
}


};