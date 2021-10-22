#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "motor.h"
#include "motorgimbal.h"
#include "motorcontrol.h"
#include "../robotcontext.h"
#include "../rcreceiver/rcreceiver.h"

using namespace std::chrono;
using namespace boost;
using namespace boost::asio;


#define TIMER_INTERVAL milliseconds(20)
//#define TIMER_INTERVAL seconds(1)


shared_ptr<MotorControl> MotorControl::create(shared_ptr<RobotContext> context) {
    return shared_ptr<MotorControl>(new MotorControl(context));
}


MotorControl::MotorControl(shared_ptr<RobotContext> context) : 
    m_initialized(false),
    m_enabled(false),
    m_mutex(new std::recursive_mutex()),
    m_timer(*(context->io()))
{
    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
    case CATEGORY_PC:
        for (int i=0; i<MOTOR_COUNT; i++) {
            m_motors.push_back(Motor::create(i, m_mutex));
        }
        break;
    }

}


MotorControl::~MotorControl() {
    cleanup();
    m_motors.clear();
}


void MotorControl::init() {
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

    for (shared_ptr<Motor> motor : m_motors) {
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
    m_timer.async_wait(boost::bind(&MotorControl::timer, this));

    m_initialized = true;
}


void MotorControl::cleanup() {
    if (!m_initialized) 
        return;
    
    m_timer.cancel();

    setEnabled(false);

    for (auto motor : m_motors) {
       motor->cleanup();
    }

    //m_fbus = NULL;

    m_initialized = false;
}





void MotorControl::brake() {
    const std::lock_guard<std::recursive_mutex> lock(*m_mutex);
    for (auto motor : m_motors) {
       motor->brake();
    }
}

void MotorControl::freeSpin() {
    const std::lock_guard<std::recursive_mutex> lock(*m_mutex);
    for (auto motor : m_motors) {
       motor->freeSpin();
    }
}



void MotorControl::setEnabled(bool enabled) {
    const std::lock_guard<std::recursive_mutex> lock(*m_mutex);

    m_enabled = enabled;

    for (auto motor : m_motors) {
        motor->setEnabled(m_enabled);
    }

    if (m_enabled) {
        rc_servo_power_rail_en(1);
        rc_motor_standby(0);
    }
    else {
        rc_servo_power_rail_en(0);
        rc_motor_standby(1);
    }
}


void MotorControl::resetOdometer() {
    const std::lock_guard<std::recursive_mutex> lock(*m_mutex);

    for (auto motor : m_motors) {
        motor->resetOdometer();
    }
}






void MotorControl::timer() {
    const std::lock_guard<std::recursive_mutex> lock(*m_mutex);

    // Update control scheme

    // Set motor gimbal angle if fbus passthrough is enabled

    // Update motors
    for (auto motor : m_motors) {
        motor->update();
    }

    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&MotorControl::timer, this));
}

