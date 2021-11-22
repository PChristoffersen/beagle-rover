#include "robot.h"

#include <iostream>
#include <exception>
#include <boost/log/trivial.hpp> 

#include <robotcontrol.h>
#include <robotcontrolext.h>

using namespace std;

namespace Robot {

Robot *Robot::m_instance { nullptr };


Robot::Robot() : 
    m_initialized { false },
    m_armed { false },
    m_context { make_shared<Context>() },
    m_rc_receiver { make_shared<RC::Receiver>(m_context) },
    m_motor_control { make_shared<Motor::Control>(m_context) },
    m_led_control { make_shared<LED::Control>(m_context) },
    m_telemetry { make_shared<Telemetry::Telemetry>(m_context, m_rc_receiver) },
    m_kinematic { make_shared<Kinematic::Kinematic>(m_context, m_motor_control, m_telemetry, m_rc_receiver) },
    m_pru_debug { make_shared<PRU::Debug>(m_context) },
    m_wifi { make_shared<System::WiFi>(m_context) }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

Robot::~Robot() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void Robot::init() 
{
    BOOST_LOG_TRIVIAL(trace) << "Initializing robot";
    if (m_instance) {
        throw domain_error("Another instance of robot have already been initialized");
    }
    m_instance = this;

    m_context->init();
    m_rc_receiver->init();
    m_telemetry->init();
    m_motor_control->init();
    m_led_control->init();
    m_kinematic->init();
    m_pru_debug->init();
    m_wifi->init();

    m_context->start();

    m_initialized = true;
}


void Robot::cleanup() 
{
    if (!m_initialized) 
        return;

    m_context->stop();

    setArmed(false);

    m_wifi->cleanup();
    m_pru_debug->cleanup();
    m_rc_receiver->cleanup();
    m_telemetry->cleanup();
    m_kinematic->cleanup();
    m_motor_control->cleanup();
    m_led_control->cleanup();

    m_context->cleanup();

    BOOST_LOG_TRIVIAL(trace) << "Robot stopped";
    m_initialized = false;

    if (m_instance==this) {
        m_instance = nullptr;
    }
}


void Robot::setArmed(bool enable) 
{
    //BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    m_armed = enable;
}


};