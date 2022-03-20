#include "power.h"

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp> 

#include <telemetry/telemetry.h>

namespace Robot::System {


PowerSource::PowerSource(Type type, const std::string &name) :
    m_type { type },
    m_name { name }
{
}


PowerSourceUnknown::PowerSourceUnknown(const std::string &name) :
    PowerSource(Type::UNKNOWN, name)
{
}


PowerSourceBattery::PowerSourceBattery(const std::string &name) :
    PowerSource(Type::BATTERY, name),
    m_charging { false },
    m_on_battery { true },
    m_jack_voltage { 0.0f },
    m_percent { 0.0f },
    m_voltage { 0.0f }
{
}

void PowerSourceBattery::set(const ::Robot::Telemetry::EventBattery &event)
{
    const guard lock(m_mutex);
    m_charging = event.charging;
    m_on_battery = event.on_battery;
    m_jack_voltage = event.jack_voltage;
    m_percent = event.percent;
    m_voltage = event.voltage;
}




Power::Power(const std::shared_ptr<Robot::Context> &context) :
    m_initialized { false },
    #if ROBOT_HAVE_BATTERY
    m_system_power { std::make_unique<PowerSourceBattery>("system")}
    #else
    m_system_power { std::make_unique<PowerSourceUnknown>("system")}
    #endif
{
}


Power::~Power() 
{
    cleanup();
}


void Power::init(const std::shared_ptr<::Robot::Telemetry::Telemetry> &telemetry)
{
    m_initialized = true;

    #if ROBOT_HAVE_BATTERY
    m_telemetry_connection = telemetry->sig_event.connect([this](const auto &e){ onTelemetryEvent(e); });
    #endif

}

void Power::cleanup()
{
    if (!m_initialized)
        return;
    m_initialized = false;

    m_telemetry_connection.disconnect();

}


void Power::onTelemetryEvent(const ::Robot::Telemetry::Event &event)
{
    #if ROBOT_HAVE_BATTERY
    if (const auto ev = dynamic_cast<const Robot::Telemetry::EventBattery*>(&event)) {
        const std::lock_guard<std::mutex> lock(m_mutex);
        if (ev->battery_id==0) {
            // Main battery
            static_cast<PowerSourceBattery*>(m_system_power.get())->set(*ev);
            notify(NOTIFY_DEFAULT);
        }
    }
    #endif
}


}

