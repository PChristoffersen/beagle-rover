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

constexpr auto STEER_PIVOT_LIMIT { 0.25f };


ControlSchemeSkid::ControlSchemeSkid(std::shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic }
{
}

ControlSchemeSkid::~ControlSchemeSkid() 
{
    cleanup();
}


void ControlSchemeSkid::init() 
{
    const guard lock(m_mutex);

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
        motor->setEnabled(false);
        motor->servo()->setEnabled(false);
    }
}


void ControlSchemeSkid::resetMotors()
{
    motorSet(FRONT_LEFT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE), 0.0f);
    motorSet(FRONT_RIGHT,Value::fromAngle(WHEEL_STRAIGHT_ANGLE), 0.0f);
    motorSet(REAR_LEFT,  Value::fromAngle(WHEEL_STRAIGHT_ANGLE), 0.0f);
    motorSet(REAR_RIGHT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE), 0.0f);
}



void ControlSchemeSkid::steer(float steering, float throttle, float aux_x, float aux_y) 
{
    const guard lock(m_mutex);
    setLastSteering(steering, throttle, aux_x, aux_y);

    auto skew = aux_x * WHEEL_MAX_TURN_ANGLE;
    if (m_orientation_reverse) {
        skew = -skew;        
    }

    float left, right;

    // Calculate turn output
    if (throttle>=0.0f) {
        left  = (steering>=0) ? 1.0f : (1.0f + steering);
        right = (steering>=0) ? (1.0f - steering) : 1.0f;
    }
    else {
        left  = (steering>=0) ? (1.0f - steering) : 1.0f;
        right = (steering>=0) ? 1.0f : (1.0f + steering);
    }

    // Scale output based on throttle
    left  *= throttle;
    right *= throttle;

    // Calculate pivot scaling
    float pivot_scale = (std::fabs(throttle)>=STEER_PIVOT_LIMIT)? 0.0 : (1.0 - std::fabs(throttle)/STEER_PIVOT_LIMIT);

    // Calculate final values
    left  = (1.0f-pivot_scale)*left  + pivot_scale*( steering);
    right = (1.0f-pivot_scale)*right + pivot_scale*(-steering);

    BOOST_LOG_TRIVIAL(info) << " steer " << boost::format("| %+.2f | %+.2f  ||  %+.2f | %+.2f |") % steering % throttle % left % right;

    motorSet(FRONT_LEFT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE - skew), left);
    motorSet(FRONT_RIGHT,Value::fromAngle(WHEEL_STRAIGHT_ANGLE + skew), right);
    motorSet(REAR_LEFT,  Value::fromAngle(WHEEL_STRAIGHT_ANGLE + skew), left);
    motorSet(REAR_RIGHT, Value::fromAngle(WHEEL_STRAIGHT_ANGLE - skew), right);
}



}