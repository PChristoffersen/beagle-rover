#include <iostream>
#include <algorithm>
#include <boost/bind.hpp>
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

static constexpr auto TIMER_INTERVAL { chrono::milliseconds(20) };
static constexpr auto MOTOR_COUNT { 4 };
static constexpr auto MOTOR_PASSTHROUGH_OFFSET { 0 };
static constexpr auto SERVO_PASSTHROUGH_OFFSET { 4 };


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
    /*
    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
        m_fbus = rc_ext_fbus_get_shm();
        break;
    case CATEGORY_PC:
        m_fbus = new shm_fbus_t;
        break;
    }
    */

    for (auto &motor : m_motors) {
       motor->init();
    }

    rc_motor_standby(0);

    /*
    // Enable power to servo and motors
    rc_servo_power_rail_en(1);
    //rc_motor_standby(0);

    uint8_t map[FBUS_CHANNELS];
    memset(map, FBUS_SERVO_UNMAPPED, sizeof(map[0]));
    map[1] = 0;
    map[2] = 1;
    map[3] = 2;
    rc_ext_fbus_set_servo_map(map);
    */

    m_timer.expires_after(TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&MotorControl::timer, this, _1));

    m_initialized = true;
}


void MotorControl::cleanup() 
{
    if (!m_initialized) 
        return;
    m_initialized = false;
    
    m_timer.cancel();

    setEnabled(false);

    for (auto &motor : m_motors) {
       motor->cleanup();
    }

    m_fbus = nullptr;
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

        // TODO Set limits

        // Set servo map
        array<uint8_t, FBUS_CHANNELS> map;
        map.fill(FBUS_SERVO_UNMAPPED);
        if (passthrough) {
            for (int i=0; i<m_motors.size(); i++) {
                map[SERVO_PASSTHROUGH_OFFSET+i] = i;
            }
        }
        rc_ext_fbus_set_servo_map(map.data());
    }
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

    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&MotorControl::timer, this, _1));
}

