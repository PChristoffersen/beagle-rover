#include "power.h"

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp> 

#include <telemetry/telemetry.h>

namespace Robot::System {

Power::Power(const std::shared_ptr<Robot::Context> &context) :
    m_initialized { false }
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


Power::~Power() 
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    cleanup();
}


void Power::init(const std::shared_ptr<::Robot::Telemetry::Telemetry> &telemetry)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    m_initialized = true;

    #if ROBOT_HAVE_Power
    m_telemetry_connection = telemetry->sig_event.connect([&](const auto &e){ telemetryEvent(e); });
    #endif

}

void Power::cleanup()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    if (!m_initialized)
        return;
    m_initialized = false;

    m_telemetry_connection.disconnect();

}


void Power::telemetryEvent(const ::Robot::Telemetry::Event &event)
{
    if (const auto ev = dynamic_cast<const Robot::Telemetry::EventBattery*>(&event)) {
        const std::lock_guard<std::mutex> lock(m_mutex);
        BOOST_LOG_TRIVIAL(info) << "Power event " << ev->percent << " { charging=" << ev->charging << ", on_Power=" << ev->on_battery << ", voltage=" << ev->voltage << ", jack=" << ev->jack_voltage << " }";
    }
}


}

