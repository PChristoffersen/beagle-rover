#include <iostream>
#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "robot.h"
#include "prudebug.h"
#include "fbus.h"
#include "motor/motorcontrol.h"
#include "led/ledcontrol.h"
#include "telemetry/telemetry.h"


using namespace boost;
using namespace boost::asio;


#define FAKE_ROBOT


Robot::Robot() : 
    m_io(new io_context()),
    m_fbus(new FBus(m_io)),
    m_motor_control(new MotorControl(m_io)),
    m_led_control(new LEDControl(m_io)),
    m_telemetry(new Telemetry(m_io)),
    m_pru_debug(new PRUDebug(m_io))
{
    std::cout << __FUNCTION__ << std::endl;
}

Robot::~Robot() {
    if (m_initialized) {
        cleanup();
    }
    std::cout << __FUNCTION__ << std::endl;
}


void Robot::init() {
    #ifndef FAKE_ROBOT
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
    m_telemetry->init();

    #endif
    m_motor_control->init();
    //m_led_control->init();

    m_motor_control->start();
    m_thread.reset(new thread(bind(&io_context::run, m_io.get())));

    m_initialized = true;
}

void Robot::cleanup() {
//    m_led_control->cleanup();
    m_motor_control->cleanup();
#ifndef FAKE_ROBOT
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

    std::cout << "Stopping thread" << std::endl;
    m_io->stop();
    m_thread->join();
    m_thread.reset();
    m_motor_control->stop();
    std::cout << "Thread stopped" << std::endl;

    m_initialized = false;
}

