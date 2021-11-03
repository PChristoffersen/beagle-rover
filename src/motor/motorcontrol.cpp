#include <iostream>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "motor.h"
#include "motorgimbal.h"
#include "motorcontrol.h"
#include "../robotcontext.h"
#include "../rcreceiver/rcreceiver.h"

using namespace std;



MotorControl::MotorControl(shared_ptr<RobotContext> context) : 
    m_initialized { false },
    m_enabled { false },
    m_passthrough { false },
    m_timer { context->io() }
{
    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
    case CATEGORY_PC:
        for (int i=0; i<MOTOR_COUNT; i++) {
            m_motors.push_back(make_unique<Motor>(i, m_mutex));
        }
        break;
    }
}


MotorControl::~MotorControl() 
{
    cleanup();
    m_motors.clear();
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void MotorControl::init() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    for (auto &motor : m_motors) {
       motor->init();
    }

    rc_motor_standby(0);

    m_timer.expires_after(TIMER_INTERVAL);
    timer_setup();

    m_initialized = true;
}


void MotorControl::cleanup() 
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





void MotorControl::brake() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    for (auto &motor : m_motors) {
       motor->brake();
    }
}

void MotorControl::freeSpin() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    for (auto &motor : m_motors) {
       motor->freeSpin();
    }
}



void MotorControl::setEnabled(bool enabled) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    m_enabled = enabled;

    for (auto &motor : m_motors) {
        motor->setEnabled(m_enabled);
        motor->gimbal().setEnabled(false);
    }

    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
        if (m_enabled) {
            rc_servo_power_rail_en(1);
            rc_motor_standby(0);
        }
        else {
            rc_servo_power_rail_en(0);
            rc_motor_standby(1);
        }
        break;
    default:
        break;
    }
}


void MotorControl::resetOdometer() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    for (auto &motor : m_motors) {
        motor->resetOdometer();
    }
}



void MotorControl::setPassthrough(bool passthrough) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (passthrough != m_passthrough) {
        m_passthrough = passthrough;
        for (auto &motor : m_motors) {
            motor->setPassthrough(passthrough);
            motor->gimbal().setPassthrough(passthrough);
        }

        // Set limits
        array<fbus_servo_limit_t, RC_SERVO_CH_MAX> limits;
        for (auto &limit : limits) {
            limit.low = PULSE_MIN;
            limit.high = PULSE_MAX;
        }
        for (const auto &motor : m_motors) {
            auto &limit = limits[motor->gimbal().getIndex()];
            limit.low = motor->gimbal().getLimitMin();
            limit.high = motor->gimbal().getLimitMax();

        }
        rc_ext_fbus_set_servo_limit(limits.data());

        // Set servo map
        array<uint8_t, FBUS_CHANNELS> map;
        map.fill(FBUS_SERVO_UNMAPPED);
        if (passthrough) {
            for (const auto &motor : m_motors) {
                auto index = motor->gimbal().getIndex();
                map[SERVO_PASSTHROUGH_OFFSET+index] = index;
            }
        }
        rc_ext_fbus_set_servo_map(map.data());
    }
}


void MotorControl::timer_setup() {
    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(
        [self_ptr=weak_from_this()] (auto &error) {
            if (auto self = self_ptr.lock()) { 
                self->timer(error); 
            }
        }
    );
}


void MotorControl::timer(boost::system::error_code error) 
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

