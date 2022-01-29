#include "skid.h"

#include <cmath>
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

#include <robotconfig.h>
#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"

using namespace Robot::Config;

namespace Robot::Kinematic {



ControlSchemeSkid::ControlSchemeSkid(std::shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

ControlSchemeSkid::~ControlSchemeSkid() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemeSkid::init() 
{
    const guard lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    resetMotors();

    const auto &motors = m_motor_control->getMotors();
    for (auto &motor : motors) {
        motor->setDuty(0.0);
        motor->setEnabled(true);
        motor->servo()->setEnabled(true);

    }

    m_initialized = true;
}


void ControlSchemeSkid::cleanup() 
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;

    const auto &motors = m_motor_control->getMotors();
    for (auto &motor : motors) {
        motor->setDuty(0.0);
    }
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


void ControlSchemeSkid::updateOrientation(Orientation orientation)
{
    const guard lock(m_mutex);
    AbstractControlScheme::updateOrientation(orientation);
    if (m_initialized) {
        resetMotors();
    }
}

void ControlSchemeSkid::resetMotors()
{
    const auto &motors = m_motor_control->getMotors();
    motorSet(FRONT_LEFT, Value::fromAngle( WHEEL_STRAIGHT_ANGLE), 0.0);
    motorSet(FRONT_RIGHT,Value::fromAngle(-WHEEL_STRAIGHT_ANGLE), 0.0);
    motorSet(REAR_LEFT,  Value::fromAngle(-WHEEL_STRAIGHT_ANGLE), 0.0);
    motorSet(REAR_RIGHT, Value::fromAngle( WHEEL_STRAIGHT_ANGLE), 0.0);
}


void ControlSchemeSkid::steer(float steering, float throttle, float aux_x, float aux_y) 
{
    const guard lock(m_mutex);

    auto skew = aux_x * WHEEL_MAX_TURN_ANGLE;

    auto left = throttle + steering/2.0;
    auto right = throttle - steering/2.0;

    left = std::clamp(left, -1.0, 1.0);
    right = std::clamp(right, -1.0, 1.0);

    BOOST_LOG_TRIVIAL(trace) << " steer " << boost::format("| %+.2f | %+.2f  ||  %+.2f | %+.2f |") % steering % throttle % left % right;


    const auto &motors = m_motor_control->getMotors();
    motorSet(FRONT_LEFT, Value::fromAngle( WHEEL_STRAIGHT_ANGLE + skew), left);
    motorSet(FRONT_RIGHT,Value::fromAngle(-WHEEL_STRAIGHT_ANGLE - skew), right);
    motorSet(REAR_LEFT,  Value::fromAngle(-WHEEL_STRAIGHT_ANGLE - skew), left);
    motorSet(REAR_RIGHT, Value::fromAngle( WHEEL_STRAIGHT_ANGLE + skew), right);
}



};