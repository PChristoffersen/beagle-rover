#include "spinning.h"

#include <boost/log/trivial.hpp>

#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"


namespace Robot::Kinematic {

constexpr auto THROTTLE_SCALE { 1.0f };


ControlSchemeSpinning::ControlSchemeSpinning(std::shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic }
{
}

ControlSchemeSpinning::~ControlSchemeSpinning() 
{
    cleanup();
}


void ControlSchemeSpinning::init() 
{
    // Set all motor angles to 0 degrees and throttle to 0
    for (auto &motor : m_motor_control->getMotors()) {
        motor->setDuty(0.0);
        motor->setEnabled(true);
        motor->servo()->setValue(Value::CENTER);
        motor->servo()->setEnabled(true);
    }

    m_initialized = true;
}


void ControlSchemeSpinning::cleanup() 
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;
}


void ControlSchemeSpinning::steer(float steering, float throttle, float aux_x, float aux_y)
{
    const guard lock(m_mutex);

    const auto duty = THROTTLE_SCALE * throttle;
    const auto &motors = m_motor_control->getMotors();
    for (auto &motor : motors) {
        motor->setDuty(duty);
    }
}


}