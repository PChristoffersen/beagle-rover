#include <iostream>
#include <boost/log/trivial.hpp>
#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "robot.h"
#include "robotcontext.h"
#include "prudebug.h"
#include "rcreceiver/rcreceiver.h"
#include "motor/motorcontrol.h"
#include "led/ledcontrol.h"
#include "telemetry/telemetry.h"
#include "kinematic/kinematic.h"




Robot::Robot() : 
    m_initialized(false),
    m_armed(false),
    m_context(std::make_shared<RobotContext>()),
    m_rc_receiver(std::make_shared<RCReceiver>(m_context)),
    m_motor_control(std::make_shared<MotorControl>(m_context)),
    m_led_control(std::make_shared<LEDControl>(m_context)),
    m_telemetry(std::make_shared<Telemetry>(m_context)),
    m_kinematic(std::make_shared<Kinematic>(m_context, m_motor_control, m_telemetry)),
    m_pru_debug(new PRUDebug(m_context))
{
}

Robot::~Robot() {
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    cleanup();
}


void Robot::init() {
    BOOST_LOG_TRIVIAL(trace) << "Initializing robot";

    m_context->init();
    m_rc_receiver->init();
    m_telemetry->init();
    m_motor_control->init();
    m_led_control->init();
    m_kinematic->init();
    //m_pru_debug->init();

    m_context->start();

    m_initialized = true;
}


void Robot::cleanup() {
    if (!m_initialized) 
        return;
    
    m_context->stop();

    setArmed(false);

    m_led_control->cleanup();
    m_motor_control->cleanup();
    m_telemetry->cleanup();
    m_rc_receiver->cleanup();
    m_kinematic->cleanup();
    //m_pru_debug->cleanup();

    m_context->cleanup();

    BOOST_LOG_TRIVIAL(trace) << "Robot stopped";
    m_initialized = false;
}


void Robot::setArmed(bool enable) {
    //BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    m_armed = enable;
}
