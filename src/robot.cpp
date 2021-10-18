#include <iostream>
#include <boost/log/trivial.hpp>
#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "robot.h"
#include "robotcontext.h"
#include "prudebug.h"
#include "rcreceiver.h"
#include "motor/motorcontrol.h"
#include "led/ledcontrol.h"
#include "telemetry/telemetry.h"


using namespace boost;
using namespace boost::asio;



Robot::Robot() : 
    m_initialized(false),
    m_armed(false),
    m_context(new RobotContext()),
    m_rc_receiver(new RCReceiver(m_context)),
    m_motor_control(new MotorControl(m_context)),
    m_led_control(new LEDControl(m_context)),
    m_telemetry(new Telemetry(m_context)),
    m_pru_debug(new PRUDebug(m_context))
{
}

Robot::~Robot() {
    cleanup();
}


void Robot::init() {
    BOOST_LOG_TRIVIAL(trace) << "Initializing robot";

    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
        initBeagleBone();
        break;
	case CATEGORY_PC:
        initPC();
        break;
    default:
        BOOST_THROW_EXCEPTION(std::runtime_error("Error initializing Unsupported model"));
    }

    m_rc_receiver->init();
    m_telemetry->init();
    m_motor_control->init();
    m_led_control->init();

    //m_motor_control->connect(m_rc_receiver);
    m_motor_control->start();

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

    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
        cleanupBeagleBone();
        break;
	case CATEGORY_PC:
        cleanupPC();
        break;
    default:
        BOOST_THROW_EXCEPTION(std::runtime_error("Error initializing Unsupported model"));

    }

    BOOST_LOG_TRIVIAL(trace) << "Robot stopped";
    m_initialized = false;
}


void Robot::initBeagleBone() {
    if (rc_adc_init()<0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error initializing ADC"));
    }
    if (rc_servo_init()<0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error initializing SERVO"));
    }
    if (rc_motor_init()<0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error initializing MOTOR"));
    }
    if (rc_ext_pru_init()<0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error initializing EXT-PRU"));
    }
    if (rc_ext_neopixel_init()<0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error initializing NEOPIXEL"));
    }
    if (rc_ext_encoder_init()<0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error initializing EXT-ENCODER"));
    }
    if (rc_ext_fbus_init()<0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error initializing FBUS"));
    }

    //rc_motor_standby(1);
    m_pru_debug->init();
}



void Robot::cleanupBeagleBone() {
    m_pru_debug->cleanup();

    rc_ext_fbus_cleanup();
    rc_ext_pru_cleanup();
    rc_ext_encoder_cleanup();
    rc_ext_neopixel_cleanup();
    rc_motor_cleanup();
    rc_servo_cleanup();
    rc_adc_cleanup();
}


void Robot::initPC() {
    
}

void Robot::cleanupPC() {

}




void Robot::setArmed(bool enable) {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    m_armed = enable;
}
