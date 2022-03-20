#include "telemetry.h"

#include <typeinfo>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <robotcontext.h>
#include "types.h"
#include "telemetrysource.h"
#include "sources/robotcontrolbattery.h"
#include "sources/robotcontrolmpu.h"



namespace Robot::Telemetry {

Telemetry::Telemetry(const std::shared_ptr<Robot::Context> &context) :
    m_initialized { false }
{

    #if ROBOT_HAVE_ROBOTCONTROL_BATTERY
    m_sources.push_back(std::make_shared<RobotControlBattery>(context));
    #endif
    #if ROBOT_HAVE_ROBOTCONTROL_MPU
    m_sources.push_back(std::make_shared<RobotControlMPU>(context));
    #endif
}


Telemetry::~Telemetry() 
{
    cleanup();
}


void Telemetry::init() 
{
    const guard lock(m_mutex);

    for (auto &source : m_sources) {
        source->init(shared_from_this());
    }
    m_initialized = true;

    // Init IMU map
    EventIMU ev("imu");
    ev.update(m_imu_values);
}


void Telemetry::cleanup() 
{
    const guard lock(m_mutex);

    if (!m_initialized)
        return;
    m_initialized = false;
    
    for (auto &source : m_sources) {
        source->cleanup();
    }
}


void Telemetry::process(const Event &event) 
{
    const guard lock(m_mutex);

    if (const auto ev = dynamic_cast<const Robot::Telemetry::EventIMU*>(&event)) {
        event.update(m_imu_values);
        notify(NOTIFY_IMU);
    }
    else if (const auto ev = dynamic_cast<const Robot::Telemetry::EventMotor*>(&event)) {
        // TODO Add to timeseries data
    }
    else if (const auto ev = dynamic_cast<const Robot::Telemetry::EventServo*>(&event)) {
        // TODO Add to timeseries data
    }

    sig_event(event);
}

#if ROBOT_HAVE_IMU
void Telemetry::process(const IMUData &data)
{
    const guard lock(m_mutex);
    sig_imu(data);

}
#endif
    
}
