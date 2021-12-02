#include "robot.h"

#include <iostream>
#include <exception>
#include <chrono>
#include <boost/log/trivial.hpp> 

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include <robottypes.h>
#include <robotcontext.h>
#include <rc/receiver.h>
#include <input/control.h>
#include <motor/control.h>
#include <led/control.h>
#include <telemetry/telemetry.h>
#include <kinematic/kinematic.h>
#include <system/prudebug.h>
#include <system/wifi.h>

using namespace std;

namespace Robot {

static constexpr auto TIMER_INTERVAL { 1s };

Robot *Robot::m_instance { nullptr };


Robot::Robot() : 
    m_initialized { false },
    m_context { make_shared<Context>() },
    m_rc_receiver { make_shared<RC::Receiver>(m_context) },
    m_motor_control { make_shared<Motor::Control>(m_context) },
    m_led_control { make_shared<LED::Control>(m_context) },
    m_telemetry { make_shared<Telemetry::Telemetry>(m_context) },
    m_kinematic { make_shared<Kinematic::Kinematic>(m_context) },
    m_input { make_shared<Input::Control>(m_context) },
    m_pru_debug { make_shared<System::PRUDebug>(m_context) },
    m_wifi { make_shared<System::WiFi>(m_context) },
    m_timer { m_context->io() },
    m_heartbeat { 0u }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

Robot::~Robot() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void Robot::init() 
{
    BOOST_LOG_TRIVIAL(info) << "Initializing robot";
    if (m_instance) {
        throw domain_error("Another instance of robot have already been initialized");
    }
    m_instance = this;

    m_context->init();
    m_telemetry->init();
    m_motor_control->init(m_rc_receiver);
    m_led_control->init();
    m_rc_receiver->init(m_telemetry);
    m_input->init(m_rc_receiver);
    m_kinematic->init(m_motor_control, m_telemetry, m_input);
    m_pru_debug->init();
    //m_wifi->init();

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

    //m_wifi->cleanup();
    m_pru_debug->cleanup();
    m_kinematic->cleanup();
    m_input->cleanup();
    m_rc_receiver->cleanup();
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


};