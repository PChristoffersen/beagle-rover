#include "robot.h"

#include <iostream>
#include <exception>
#include <chrono>
#include <boost/log/trivial.hpp>

#include <robottypes.h>
#include <robotcontext.h>
#include <rc/receiver.h>
#include <input/control.h>
#include <motor/control.h>
#include <led/control.h>
#include <telemetry/telemetry.h>
#include <kinematic/kinematic.h>
#include <system/network.h>
#include <system/power.h>
#include <hardware/beaglebone/prudebug.h>

using namespace std::literals;

namespace Robot {

static constexpr auto TIMER_INTERVAL { 1s };

Robot *Robot::m_instance { nullptr };



Robot::Robot() : 
    m_log_init { boost::log::trivial::debug },
    m_initialized { false },
    m_context { std::make_shared<Context>() },
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    m_pru_debug { std::make_shared<Hardware::Beaglebone::PRUDebug>(m_context) },
    #endif
    #if ROBOT_HAVE_RC
    m_rc_receiver { std::make_shared<RC::Receiver>(m_context) },
    #endif
    m_motor_control { std::make_shared<Motor::Control>(m_context) },
    m_led_control { std::make_shared<LED::Control>(m_context) },
    m_telemetry { std::make_shared<Telemetry::Telemetry>(m_context) },
    m_kinematic { std::make_shared<Kinematic::Kinematic>(m_context) },
    m_input { std::make_shared<Input::Control>(m_context) },
    m_network { std::make_shared<System::Network>(m_context) },
    m_power { std::make_shared<System::Power>(m_context) },
    m_timer { m_context->io() },
    m_heartbeat { 0u }
{
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

Robot::~Robot() 
{
    cleanup();
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}





void Robot::init() 
{
    BOOST_LOG_TRIVIAL(info) << "Initializing robot";
    if (m_instance) {
        BOOST_THROW_EXCEPTION(std::domain_error("Another instance of robot have already been initialized"));
    }
    m_instance = this;

    m_context->init();
    m_telemetry->init();
    m_motor_control->init(m_telemetry);
    m_led_control->init();
    if (m_rc_receiver) {
        m_rc_receiver->init(m_telemetry);
    }
    m_input->init(m_rc_receiver);
    m_kinematic->init(m_motor_control, m_led_control, m_telemetry, m_input);
    m_network->init();
    m_power->init(m_telemetry);
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    m_pru_debug->init();
    #endif

    m_heartbeat = 0u;
    m_timer.expires_after(0s);
    timerSetup();

    m_context->start();

    m_initialized = true;
}


void Robot::cleanup() 
{
    if (!m_initialized) 
        return;

    m_initialized = false;

    m_timer.cancel();

    m_context->stop();

    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    m_pru_debug->cleanup();
    #endif
    m_power->cleanup();
    m_network->cleanup();
    m_kinematic->cleanup();
    m_input->cleanup();
    if (m_rc_receiver) {
        m_rc_receiver->cleanup();
    }
    m_led_control->cleanup();
    m_motor_control->cleanup();
    m_telemetry->cleanup();

    m_context->cleanup();

    BOOST_LOG_TRIVIAL(info) << "Robot stopped";
    if (m_instance==this) {
        m_instance = nullptr;
    }
}


void Robot::timerSetup() 
{
    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(
        [self_ptr=this->weak_from_this()] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            if (auto self = self_ptr.lock()) { 
                self->timer(); 
            }
        }
    );
}

void Robot::timer()
{
    ++m_heartbeat;
    timerSetup();
}


}




