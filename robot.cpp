#include <iostream>
#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "robot.h"
#include "prudebug.h"
#include "motorcontrol.h"
#include "led/ledcontrol.h"
#include "telemetry/telemetry.h"


using namespace std;
using namespace boost;
using namespace boost::asio;


Robot::Robot(io_context &io) : 
    m_pru_debug(new PRUDebug(io)),
    m_motor_control(new MotorControl(io)),
    m_led_control(new LEDControl(io)),
    m_telemetry(new Telemetry(io))
{
}

Robot::~Robot() {
    if (m_initialized) {
        cleanup();
    }
}


void Robot::init() {
    #if 1
    if (rc_adc_init()<0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing ADC"));
    }
    if (rc_servo_init()<0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing SERVO"));
    }
    if (rc_motor_init()<0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing MOTOR"));
    }
    if (rc_ext_pru_init()<0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing EXT-PRU"));
    }
    if (rc_ext_neopixel_init()<0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing NEOPIXEL"));
    }
    if (rc_ext_encoder_init()<0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing EXT-ENCODER"));
    }
    if (rc_ext_fbus_init()<0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing FBUS"));
    }

    //rc_motor_standby(1);

    m_pru_debug->init();
    m_telemetry->init();
    #endif

    m_motor_control->init();
    m_led_control->init();

    Component::init();
}

void Robot::cleanup() {
    m_led_control->cleanup();
    m_motor_control->cleanup();
#if 1
    m_telemetry->cleanup();
    m_pru_debug->cleanup();

    rc_ext_fbus_cleanup();
    rc_ext_pru_cleanup();
    rc_ext_encoder_cleanup();
    rc_ext_neopixel_cleanup();
    rc_motor_cleanup();
    rc_servo_cleanup();
    rc_adc_cleanup();
#endif

    Component::cleanup();
}
