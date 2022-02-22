#include "robotcontext.h"

#include <iostream>
#include <functional>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp> 

#include <robotconfig.h>
#include <hardware/nooppower.h>
#include <hardware/proxypower.h>
#include <hardware/beaglebone/servopower.h>
#include <hardware/beaglebone/motorpower.h>


using namespace std::literals;

namespace Robot {


static constexpr auto CONTEXT_THREAD_NICE { -20 };


Context::Context() : 
    m_initialized { false },
    m_started { false },
    m_motor_power_enabled { false },
    m_servo_power_enabled { false },
    m_led_power_enabled { false },
    m_rc_power_enabled { false }
{
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;

    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    rc_model();
    #endif
}


Context::~Context() 
{
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    stop();
    cleanup();
}




void Context::init() 
{
    BOOST_LOG_TRIVIAL(info) << "Init context";
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

    sig_thread.disconnect_all_slots();
    
    BOOST_LOG_TRIVIAL(info) << "Cleanup context";
}


#if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
void Context::initPlatform() 
{
    auto uid = geteuid();
    if (uid!=0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Robot library requires root"));
    }

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

    m_motor_power = std::make_shared<::Robot::Hardware::Beaglebone::MotorPower>();
    m_servo_power = std::make_shared<::Robot::Hardware::Beaglebone::ServoPower>();
    m_led_power = std::make_shared<Robot::Hardware::NoopPower>();
    m_rc_power = m_servo_power;

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

    m_servo_power = nullptr;
    m_motor_power = nullptr;
    m_led_power = nullptr;
    m_rc_power = nullptr;
}
#endif


#if ROBOT_PLATFORM == ROBOT_PLATFORM_PC

void Context::initPlatform() 
{
    m_servo_power = std::make_shared<Robot::Hardware::NoopPower>();
    m_motor_power = std::make_shared<Robot::Hardware::NoopPower>();
    m_led_power = std::make_shared<Robot::Hardware::NoopPower>();
    m_rc_power = std::make_shared<Robot::Hardware::NoopPower>();
}

void Context::cleanupPlatform() 
{
    m_servo_power = nullptr;
    m_motor_power = nullptr;
    m_led_power = nullptr;
    m_rc_power = nullptr;
}
#endif


void Context::start() 
{
    const guard lock(m_mutex);

    BOOST_LOG_TRIVIAL(info) << "Starting thread";
    m_thread = std::make_shared<std::thread>([&]{ 
        auto val = nice(CONTEXT_THREAD_NICE);
        if (val != CONTEXT_THREAD_NICE) {
            #if ROBOT_PLATFORM != ROBOT_PLATFORM_PC
            BOOST_LOG_TRIVIAL(warning) << "Failed to set context thread nice value (" << val << ")";
            #endif
        }
        sig_thread(true);
        m_io.run(); 
        sig_thread(false);
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


void Context::setPowerEnabled(std::shared_ptr<::Robot::Hardware::AbstractPower> &power, power_signal_type &sig_power, bool enabled, bool &state, const char *name)
{
    auto cnt = power->setEnabled(enabled);
    if (cnt>=1) {
        if (!state) {
            state = true;
            BOOST_LOG_TRIVIAL(info) << "Enabling " << name << " power";
            sig_power(true);
        }
    }
    else if (cnt==0) {
        if (state) {
            state = false;
            BOOST_LOG_TRIVIAL(info) << "Disabling " << name << " power";
            sig_power(false);
        }
    }
}

void Context::motorPower(bool enable) 
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;

    setPowerEnabled(m_motor_power, sig_motor_power, enable, m_motor_power_enabled, "motor");
}


void Context::servoPower(bool enable) 
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;

    setPowerEnabled(m_servo_power, sig_servo_power, enable, m_servo_power_enabled, "servo");
}


void Context::ledPower(bool enable) 
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;

    setPowerEnabled(m_led_power, sig_led_power, enable, m_led_power_enabled, "led");
}

void Context::rcPower(bool enable) 
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;

    setPowerEnabled(m_rc_power, sig_rc_power, enable, m_rc_power_enabled, "rc");
}



}