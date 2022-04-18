#include "motors.h"

#include <motor/control.h>
#include <motor/motor.h>
#include <robotcontext.h>

namespace Robot::Telemetry {

Motors::Motors(const std::shared_ptr<Motor::Control> &motor_control) :
    m_motor_control { motor_control },
    m_event { "motors" }
{

}


Motors::~Motors()
{
    cleanup();
}


void Motors::init(const std::shared_ptr<Telemetry> &telemetry)
{
    m_initialized = true;
    AbstractSource::init(telemetry);

    if (auto control = m_motor_control.lock()) {
        m_connection = control->sig_motor.connect([this](const auto &motors) { onMotorsUpdated(motors); });
    }
}


void Motors::cleanup()
{
    if (!m_initialized)
        return;
    m_initialized = false;

    m_connection.disconnect();

    AbstractSource::cleanup();
}


void Motors::onMotorsUpdated(const Motor::MotorList &motors)
{
    for (auto i = 0u; i<motors.size(); i++) {
        m_event.duty[i] = motors[i]->getDuty();
        m_event.rpm[i] = motors[i]->getRPM();
        m_event.rpm_target[i] = motors[i]->getTargetRPM();
    }
    sendEvent(m_event);
}



}