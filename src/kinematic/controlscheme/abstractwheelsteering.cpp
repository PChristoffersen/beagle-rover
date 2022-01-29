#include "abstractwheelsteering.h"

#include <cmath>
#include <algorithm>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"

using namespace Robot::Config;

namespace Robot::Kinematic {

AbstractWheelSteering::AbstractWheelSteering(std::shared_ptr<Kinematic> kinematic, float wheel_base_factor) :
    AbstractControlScheme { kinematic },
    m_wheel_base_factor { wheel_base_factor }
{

}


void AbstractWheelSteering::init() 
{
    const guard lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    resetMotors();

    const auto &motors = m_motor_control->getMotors();
    for (auto &motor : motors) {
        motor->setDuty(0.0);
        motor->setEnabled(false);
        motor->servo()->setEnabled(true);

    }

    m_initialized = true;
}


void AbstractWheelSteering::cleanup() 
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


void AbstractWheelSteering::updateOrientation(Orientation orientation)
{
    const guard lock(m_mutex);
    AbstractControlScheme::updateOrientation(orientation);
    if (m_initialized) {
        resetMotors();
    }
}


void AbstractWheelSteering::resetMotors()
{
    const auto &motors = m_motor_control->getMotors();
    motorSet(FRONT_LEFT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE), 0.0);
    motorSet(FRONT_RIGHT,Value::fromAngle(-WHEEL_STRAIGHT_ANGLE), 0.0);
    motorSet(REAR_LEFT,  Value::fromAngle(-WHEEL_STRAIGHT_ANGLE), 0.0);
    motorSet(REAR_RIGHT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE), 0.0);
}


void AbstractWheelSteering::steer(float steering, float throttle, float aux_x, float aux_y) 
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "  steering="<<steering;

    const auto asteering = abs(steering);
    const auto &motors = m_motor_control->getMotors();
    
    // Just go straight (to avoid infinite numbers for turning radius)
    if (asteering < 0.01) {
        resetMotors();
        return;
    }

    // Inner angle is just the fraction of steering times max turn angle
    auto inner_angle = asteering * WHEEL_MAX_TURN_ANGLE;

    // Inner wheel turning circle center from inner wheels centerpoint.
    auto turning_circle_dist = tan(M_PI_2-inner_angle) * m_wheel_base_factor;

    // Calculate outer angle
    auto outer_angle = atan(m_wheel_base_factor/(turning_circle_dist+WHEEL_BASE_MM));

    if (steering > 0.0) {
        setMotors(-outer_angle, inner_angle);
    }
    else {
        setMotors(inner_angle, -outer_angle);
    }
}


};