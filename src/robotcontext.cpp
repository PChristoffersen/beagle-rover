#include "robotcontext.h"

#include <iostream>
#include <functional>
#include <boost/bind.hpp>
#include <boost/log/core.hpp> 
#include <boost/log/trivial.hpp> 
#include <boost/log/expressions.hpp> 
#include <robotcontrol.h>
#include <robotcontrolext.h>

using namespace std;

namespace Robot {

Context::Context() : 
    m_initialized { false },
    m_started { false },
    m_servo_power_rail_cnt { 0 }
{
    initLogging();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;

    rc_model();
}


Context::~Context() 
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    stop();
    cleanup();
}


void Context::initLogging() {
    namespace logging = boost::log;
    namespace keywords = boost::log::keywords;

    #if 0
    constexpr auto level = logging::trivial::trace;
    #else
    constexpr auto level = logging::trivial::debug;
    #endif

    logging::core::get()->set_filter(
        logging::trivial::severity >= level
    ); 
}



void Context::init() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
        initBeagleBone();
        break;
	case CATEGORY_PC:
        initPC();
        break;
    default:
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing Unsupported model"));
    }
    m_initialized = true;
}


void Context::cleanup() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    if (!m_initialized)
        return;
    m_initialized = false;

    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
        cleanupBeagleBone();
        break;
	case CATEGORY_PC:
        cleanupPC();
        break;
    default:
        BOOST_THROW_EXCEPTION(runtime_error("Error initializing Unsupported model"));

    }

    // Drain the io_context and execute any remaining tasks
    m_io.restart();
    m_io.run_for(chrono::seconds(10));
    m_io.stop();
}



void Context::initBeagleBone() 
{
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

    rc_motor_standby(1);
    rc_servo_power_rail_en(0);
}


void Context::cleanupBeagleBone() 
{
    rc_ext_fbus_cleanup();
    rc_ext_pru_cleanup();
    rc_ext_encoder_cleanup();
    rc_ext_neopixel_cleanup();
    rc_motor_cleanup();
    rc_servo_cleanup();
    rc_adc_cleanup();
}


void Context::initPC() 
{
}

void Context::cleanupPC() 
{
}


void Context::start() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    BOOST_LOG_TRIVIAL(info) << "Starting thread";
    m_thread = make_shared<thread>( [&]{ m_io.run(); } );
    m_started = true;
}


void Context::stop() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    if (!m_started)
        return;
    
    BOOST_LOG_TRIVIAL(info) << "Stopping thread";
    m_io.stop();
    m_thread->join();
    m_thread = nullptr;
    BOOST_LOG_TRIVIAL(info) << "Thread stopped";

    m_started = false;
}


void Context::motorPower(bool enable) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (!m_initialized)
        return;

    if (enable) {
        m_motor_power_rail_cnt++;
        BOOST_LOG_TRIVIAL(info) << "MotorPowerRail++ " << m_motor_power_rail_cnt;
        if (m_motor_power_rail_cnt==1) {
            rc_motor_standby(0);
        }
    }
    else {
        m_motor_power_rail_cnt--;
        BOOST_LOG_TRIVIAL(info) << "MotorPowerRail-- " << m_motor_power_rail_cnt;
        if (m_motor_power_rail_cnt==0) {
            rc_motor_standby(1);
        }
    }
}


void Context::servoPower(bool enable) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (!m_initialized)
        return;
        
    if (enable) {
        m_servo_power_rail_cnt++;
        BOOST_LOG_TRIVIAL(info) << "ServoPowerRail++ " << m_servo_power_rail_cnt;
        if (m_servo_power_rail_cnt==1) {
            rc_servo_power_rail_en(1);
        }
    }
    else {
        assert(m_servo_power_rail_cnt>0);
        m_servo_power_rail_cnt--;
        BOOST_LOG_TRIVIAL(info) << "ServoPowerRail-- " << m_servo_power_rail_cnt;
        if (m_servo_power_rail_cnt==0) {
            rc_servo_power_rail_en(0);
        }
    }
}


void Context::rcPower(bool enable) 
{
    // RC Receiver is powered from the servo rail
    servoPower(enable);
}



};