#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "motor.h"
#include "motorcontrol.h"

using namespace std::chrono;
using namespace boost;
using namespace boost::asio;


#define TIMER_INTERVAL milliseconds(200)
//#define TIMER_INTERVAL seconds(1)

MotorControl::MotorControl(shared_ptr<io_context> io) : 
    Component(io),
    m_started(false),
    m_timer(*io.get())
{
    std::cout << "MotorControl()"  << std::endl;
    for (int i=0; i<MOTOR_COUNT; i++) {
        m_motors.push_back(shared_ptr<Motor>(new Motor(i, io)));
    }
    std::cout << "MotorControl() <<"  << std::endl;
}


void MotorControl::init() {
    std::cout << "MotorControl::init"  << std::endl;
    m_fbus = rc_ext_fbus_get_channels();

    for (shared_ptr<Motor> motor : m_motors) {
       motor->init();
    }

    Component::init();    
    std::cout << "MotorControl::init <<"  << std::endl;
}


void MotorControl::cleanup() {
    if (m_started) {
        stop();
    }
    for (shared_ptr<Motor> motor : m_motors) {
       motor->cleanup();
    }

    m_fbus = NULL;
    Component::cleanup();    
}


void MotorControl::start() {
#if 0
    // Enable power to servo and motors
    rc_servo_power_rail_en(1);
    //rc_motor_standby(0);

    uint8_t map[FBUS_CHANNELS];
    memset(map, FBUS_SERVO_UNMAPPED, sizeof(map[0]));
    map[1] = 0;
    map[2] = 1;
    map[3] = 2;
    rc_ext_fbus_set_servo_map(map);

    m_last_counter = m_fbus->counter;
#endif
    m_timer.expires_after(TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&MotorControl::timer, this));
    m_started = true;
}

void MotorControl::stop() {
    m_timer.cancel();
#if 0
    // Turn off power to servo and motors
    rc_servo_power_rail_en(0);
    //rc_motor_standby(1);

#endif
    m_started = false;
}



void MotorControl::timer() {
    #if 0
    for (shared_ptr<Motor> motor : m_motors) {
       motor->update();
    }

    /*
    for (int i=0; i<MOTOR_COUNT; i++) {
        m_motor[i].update();
    }
    */
    //m_motor1.update();



    //rc_motor_set(0, (m_fbus->channels[0]-500)/2000.0);
    #endif
    std::cout << "MotorContro::timer" << std::endl;


    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&MotorControl::timer, this));
}

