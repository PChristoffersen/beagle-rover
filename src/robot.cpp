#include "robot.h"

#include <iostream>
#include <exception>
#include <boost/log/trivial.hpp> 

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "robotcontext.h"
#include "rcreceiver/rcreceiver.h"
#include "motor/motorcontrol.h"
#include "led/ledcontrol.h"
#include "telemetry/telemetry.h"
#include "kinematic/kinematic.h"
#include "debug/prudebug.h"

using namespace std;

Robot *Robot::m_instance { nullptr };


Robot::Robot() : 
    m_initialized { false },
    m_armed { false },
    m_context { make_shared<RobotContext>() },
    m_rc_receiver { make_shared<RCReceiver>(m_context) },
    m_motor_control { make_shared<MotorControl>(m_context) },
    m_led_control { make_shared<LEDControl>(m_context) },
    m_telemetry { make_shared<Telemetry>(m_context) },
    m_kinematic { make_shared<Kinematic>(m_context, m_motor_control, m_telemetry, m_rc_receiver) },
    m_pru_debug { make_shared<PRUDebug>(m_context) }
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

    m_context->start();

    m_initialized = true;
}


void Robot::cleanup() 
{
    if (!m_initialized) 
        return;

    m_context->stop();

    setArmed(false);

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
