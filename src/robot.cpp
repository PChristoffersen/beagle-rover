#include "robot.h"

#include <iostream>
#include <exception>
#include <chrono>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>


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
    m_initialized { false },
    m_log_config {},
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
    #if ROBOT_HAVE_WIFI
    m_wifi { std::make_shared<System::WiFi>(m_context) },
    #endif
    m_power { std::make_shared<System::Power>(m_context) },
    m_timer { m_context->io() },
    m_heartbeat { 0u }
{

//    m_context = std::make_shared<Context>();

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
        BOOST_THROW_EXCEPTION(std::domain_error("Another instance of robot have already been initialized"));
    }
    m_instance = this;

    m_context->init();
    m_telemetry->init();
    m_motor_control->init(m_telemetry);
    m_led_control->init();
    #if ROBOT_HAVE_RC
    m_rc_receiver->init(m_telemetry);
    m_input->init(m_rc_receiver);
    #else
    m_input->init();
    #endif
    m_kinematic->init(m_motor_control, m_led_control, m_telemetry, m_input);
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    m_pru_debug->init();
    #endif
    #if ROBOT_HAVE_WIFI
    m_wifi->init();
    #endif
    m_power->init(m_telemetry);

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

    m_power->cleanup();
    #if ROBOT_HAVE_WIFI
    m_wifi->cleanup();
    #endif
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    m_pru_debug->cleanup();
    #endif
    m_kinematic->cleanup();
    m_input->cleanup();
    #if ROBOT_HAVE_RC
    m_rc_receiver->cleanup();
    #endif
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


Robot::LogConfig::LogConfig() 
{
    namespace logging = boost::log;
    namespace keywords = boost::log::keywords;
    namespace expr = boost::log::expressions;
 
    logging::add_common_attributes();

    auto sink = logging::add_console_log();
    auto formatter =
            expr::stream
            << "["
            << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%H:%M:%S.%f")
            << "] ["
            << expr::attr<boost::log::attributes::current_thread_id::value_type>("ThreadID") 
            << "] ["
            << boost::log::trivial::severity
            << "]  "
            << expr::message;

    sink->set_formatter(formatter);
    sink->locked_backend()->auto_flush(true);

    #if 0
    constexpr auto level = logging::trivial::trace;
    #else
    constexpr auto level = logging::trivial::debug;
    #endif

    logging::core::get()->set_filter(
        logging::trivial::severity >= level
    );


    BOOST_LOG_TRIVIAL(info) << "Logging initialized";
}


}




