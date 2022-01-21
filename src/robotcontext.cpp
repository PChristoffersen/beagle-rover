#include "robotcontext.h"

#include <iostream>
#include <functional>
#include <boost/bind.hpp>
#include <boost/log/core.hpp> 
#include <boost/log/trivial.hpp> 
#include <boost/log/expressions.hpp> 

#include <robotconfig.h>

using namespace std::literals;

namespace Robot {


static constexpr auto CONTEXT_THREAD_NICE { -20 };


Context::Context() : 
    m_initialized { false },
    m_started { false },
    m_motor_power_rail_cnt { 0 },
    m_servo_power_rail_cnt { 0 },
    m_rc_power_rail_cnt { 0 }
{
    initLogging();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;

    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    rc_model();
    #endif
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
    const guard lock(m_mutex);

    initPlatform();

    m_initialized = true;
}


void Context::cleanup() 
{
    const guard lock(m_mutex);

    if (!m_initialized)
        return;
    m_initialized = false;

    cleanupPlatform();

    // Drain the io_context and execute any remaining tasks
    m_io.restart();
    m_io.run_for(10s);
    m_io.stop();
}


#if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
void Context::initPlatform() 
{
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

    rc_motor_standby(1);
    rc_servo_power_rail_en(0);
}

void Context::cleanupPlatform() 
{
    rc_ext_fbus_cleanup();
    rc_ext_pru_cleanup();
    rc_ext_encoder_cleanup();
    rc_ext_neopixel_cleanup();
    rc_motor_cleanup();
    rc_servo_cleanup();
    rc_adc_cleanup();
    rc_led_cleanup();
}
#endif


#if ROBOT_PLATFORM == ROBOT_PLATFORM_PC
void Context::initPlatform() 
{
}

void Context::cleanupPlatform() 
{
}
#endif


void Context::start() 
{
    const guard lock(m_mutex);

    BOOST_LOG_TRIVIAL(info) << "Starting thread";
    m_thread = std::make_shared<std::thread>([&]{ 
        auto val = nice(CONTEXT_THREAD_NICE);
        if (val != CONTEXT_THREAD_NICE) {
            BOOST_LOG_TRIVIAL(warning) << "Failed to set context thread nice value (" << val << ")";
        }
        m_io.run(); 
    });
    m_started = true;
}


void Context::stop() 
{
    const guard lock(m_mutex);

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
    const guard lock(m_mutex);
    if (!m_initialized)
        return;

    if (enable) {
        m_motor_power_rail_cnt++;
        if (m_motor_power_rail_cnt==1) {
            BOOST_LOG_TRIVIAL(info) << "Enabling motor power";
            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            rc_motor_standby(0);
            #endif
            sig_motor_power(true);
        }
    }
    else {
        m_motor_power_rail_cnt--;
        if (m_motor_power_rail_cnt==0) {
            BOOST_LOG_TRIVIAL(info) << "Disabling motor power";
            sig_motor_power(false);
            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            rc_motor_standby(1);
            #endif
        }
    }
}


void Context::servoPower(bool enable) 
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
        
    if (enable) {
        m_servo_power_rail_cnt++;
        if (m_servo_power_rail_cnt==1) {
            BOOST_LOG_TRIVIAL(info) << "Enabling servo power";
            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            rc_servo_power_rail_en(1);
            #endif
            sig_servo_power(true);
            sig_rc_power(true);
        }
    }
    else {
        assert(m_servo_power_rail_cnt>0);
        m_servo_power_rail_cnt--;
        if (m_servo_power_rail_cnt==0) {
            BOOST_LOG_TRIVIAL(info) << "Disabling servo power";
            sig_rc_power(false);
            sig_servo_power(false);
            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            rc_servo_power_rail_en(0);
            #endif
        }
    }
}


void Context::rcPower(bool enable) 
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;

    if (enable) {
        m_rc_power_rail_cnt++;
        if (m_rc_power_rail_cnt==1) {
            BOOST_LOG_TRIVIAL(info) << "Enabling rc power";
            #if ROBOT_BOARD == ROBOT_BOARD_BEAGLEBONE_BLUE
            // RC Receiver is powered from the servo rail
            servoPower(true);
            #endif
            sig_rc_power(true);
        }
    }
    else {
        assert(m_rc_power_rail_cnt>0);
        m_rc_power_rail_cnt--;
        if (m_rc_power_rail_cnt==0) {
            BOOST_LOG_TRIVIAL(info) << "Disabling rc power";
            #if ROBOT_BOARD == ROBOT_BOARD_BEAGLEBONE_BLUE
            // RC Receiver is powered from the servo rail
            servoPower(false);
            #endif
            sig_rc_power(false);
        }
    }
}



};