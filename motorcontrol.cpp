#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "motorcontrol.h"

using namespace boost;
using namespace boost::asio;


#define TIMER_INTERVAL 100

MotorControl::MotorControl(io_context &io) : 
    m_started(false),
    m_timer(io)
{
}


void MotorControl::init() {
    m_fbus = rc_ext_fbus_get_channels();

    Component::init();    
}


void MotorControl::cleanup() {
    if (m_started) {
        stop();
    }
    m_fbus = NULL;
    Component::cleanup();    
}


void MotorControl::start() {
    // Enable power to servo and motors
    rc_servo_power_rail_en(1);
    //rc_motor_standby(0);

    uint8_t map[FBUS_CHANNELS];
    memset(map, FBUS_SERVO_UNMAPPED, sizeof(map[0]));
    map[1] = 0;
    map[2] = 1;
    map[3] = 2;
    rc_ext_fbus_set_servo_map(map);

    m_timer.expires_after(chrono::milliseconds(TIMER_INTERVAL));
    m_timer.async_wait(boost::bind(&MotorControl::timer, this));
    m_started = true;
}

void MotorControl::stop() {
    m_timer.cancel();

    // Turn off power to servo and motors
    rc_servo_power_rail_en(0);
    //rc_motor_standby(1);
    m_started = false;
}



void MotorControl::timer() {
    std::cout << format("%+04x f=%+02x  r=%+02x   ") % (uint32_t)m_fbus->counter % (uint32_t)m_fbus->flags % (uint32_t)m_fbus->rssi;
    for (int i=0; i<8; i++) {
        uint32_t ch = m_fbus->channels[i];
        std::cout << format("%+4d |") % ch;
    }
    std::cout << "  " << (m_fbus->channels[0]-500) << "   "  << ((m_fbus->channels[0]-500)/2000.0) << "          ";
    std::cout << "\r" << std::flush;

    rc_motor_set(0, (m_fbus->channels[0]-500)/2000.0);

    m_timer.expires_at(m_timer.expiry() + chrono::milliseconds(TIMER_INTERVAL));
    m_timer.async_wait(boost::bind(&MotorControl::timer, this));
}
