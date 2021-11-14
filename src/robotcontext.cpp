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


RobotContext::RobotContext() : 
    m_initialized { false },
    m_started { false },
    m_power_enabled { false }
{
    initLogging();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;

    rc_model();
}


RobotContext::~RobotContext() 
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    stop();
    cleanup();
}


void RobotContext::initLogging() {
    namespace logging = boost::log;
    namespace keywords = boost::log::keywords;

    #if 1
    constexpr auto level = logging::trivial::trace;
    #else
    constexpr auto level = logging::trivial::debug;
    #endif

    logging::core::get()->set_filter(
        logging::trivial::severity >= level
    ); 
}



void RobotContext::init() 
{
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


void RobotContext::cleanup() 
{
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



void RobotContext::initBeagleBone() 
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

    rc_servo_power_rail_en(1);
}


void RobotContext::cleanupBeagleBone() 
{
    rc_ext_fbus_cleanup();
    rc_ext_pru_cleanup();
    rc_ext_encoder_cleanup();
    rc_ext_neopixel_cleanup();
    rc_motor_cleanup();
    rc_servo_cleanup();
    rc_adc_cleanup();
}


void RobotContext::initPC() 
{
}

void RobotContext::cleanupPC() 
{
}


void RobotContext::start() 
{
    BOOST_LOG_TRIVIAL(info) << "Starting thread";
    m_thread = make_shared<thread>( [&]{ m_io.run(); } );
    m_started = true;
}


void RobotContext::stop() 
{
    if (!m_started)
        return;
    
    BOOST_LOG_TRIVIAL(info) << "Stopping thread";
    m_io.stop();
    m_thread->join();
    m_thread.reset();
    BOOST_LOG_TRIVIAL(info) << "Thread stopped";

    m_started = false;
}

