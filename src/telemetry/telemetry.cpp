#include "telemetry.h"

#include <typeinfo>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <robotcontext.h>
#include "types.h"
#include "sources/robotcontrolbattery.h"
#include "sources/robotcontrolmpu.h"



namespace Robot::Telemetry {

Telemetry::Telemetry(const std::shared_ptr<Robot::Context> &context) :
    m_initialized { false },
    m_values_version { 0 }
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
    for (auto &source : m_sources) {
        source->init(shared_from_this());
    }
    m_initialized = true;
}


void Telemetry::cleanup() 
{
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
    event.update(m_values);
    m_values_version++;
    sig_event(event);
}

#if ROBOT_HAVE_MPU
void Telemetry::process(const MPUData &data)
{
    const guard lock(m_mutex);
    sig_mpu(data);

}
#endif
    
}
