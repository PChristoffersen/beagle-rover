#include "odometer.h"

#include <kinematic/kinematic.h>
#include <robotcontext.h>

namespace Robot::Telemetry {

Odometer::Odometer(const std::shared_ptr<Kinematic::Kinematic> &kinematic) :
    m_kinematic { kinematic },
    m_event { "odometer" }
{

}


Odometer::~Odometer()
{
    cleanup();
}


void Odometer::init(const std::shared_ptr<Telemetry> &telemetry)
{
    m_initialized = true;
    AbstractSource::init(telemetry);

    if (auto kinematic = m_kinematic.lock()) {
        m_connection = kinematic->sig_odometer.connect([this](auto odometer) { onOdometerUpdated(odometer); });
    }
}


void Odometer::cleanup()
{
    if (!m_initialized)
        return;
    m_initialized = false;

    m_connection.disconnect();

    AbstractSource::cleanup();
}


void Odometer::onOdometerUpdated(Kinematic::Kinematic::odometer_type odometer)
{
    m_event.value = odometer;
    sendEvent(m_event);
}



}