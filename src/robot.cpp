#include <iostream>
#include <boost/log/trivial.hpp>
#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "robot.h"
#include "robotcontext.h"
#include "prudebug.h"
#include "rcreceiver/rcreceiver.h"
#include "motor/motorcontrol.h"
#include "led/ledcontrol.h"
#include "telemetry/telemetry.h"
#include "kinematic/kinematic.h"


using namespace boost;
using namespace boost::asio;



Robot::Robot() : 
    m_initialized(false),
    m_armed(false),
    m_context(RobotContext::create()),
    m_rc_receiver(RCReceiver::create(m_context)),
    m_motor_control(MotorControl::create(m_context)),
    m_led_control(LEDControl::create(m_context)),
    m_telemetry(Telemetry::create(m_context)),
    m_kinematic(Kinematic::create(m_context, m_motor_control, m_telemetry)),
    m_pru_debug(new PRUDebug(m_context))
{
}

Robot::~Robot() {
    cleanup();
}


void Robot::init() {
    BOOST_LOG_TRIVIAL(trace) << "Initializing robot";

    m_context->init();
    m_rc_receiver->init();
    m_telemetry->init();
    m_motor_control->init();
    m_led_control->init();
    //m_pru_debug->init();

    m_kinematic->init();

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
    //m_pru_debug->cleanup();

    m_context->cleanup();

    BOOST_LOG_TRIVIAL(trace) << "Robot stopped";
    m_initialized = false;
}


void Robot::setArmed(bool enable) {
    //BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    m_armed = enable;
}
