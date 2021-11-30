#include "standard.h"

#include <cmath>
#include <algorithm>
#include <boost/log/trivial.hpp>

#include "../../motor/motor.h"
#include "../../motor/motorservo.h"
#include "../../motor/motorcontrol.h"
#include "../kinematictypes.h"

using namespace std;

namespace Robot::Kinematic {



ControlSchemeStandard::ControlSchemeStandard(shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

ControlSchemeStandard::~ControlSchemeStandard() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemeStandard::init() 
{
    const lock_guard<mutex> lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    const auto &motors = m_motor_control->getMotors();
    motors[0]->servo()->setValue(Value::fromAngleRadians(WHEEL_STRAIGHT_ANGLE));
    motors[1]->servo()->setValue(Value::fromAngleRadians(-WHEEL_STRAIGHT_ANGLE));
    motors[2]->servo()->setValue(Value::fromAngleRadians(-WHEEL_STRAIGHT_ANGLE));
    motors[3]->servo()->setValue(Value::fromAngleRadians(WHEEL_STRAIGHT_ANGLE));

    for (auto &motor : motors) {
        //motor->setDuty(0.0);
        //motor->setEnabled(true);
        motor->servo()->setEnabled(true);

    }

    m_initialized = true;
}


void ControlSchemeStandard::cleanup() 
{
    const lock_guard<mutex> lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}



void ControlSchemeStandard::steer(double steering, double throttle, double aux_x, double aux_y) 
{
    const lock_guard<mutex> lock(m_mutex);
    steerAll(steering, throttle);
}

void ControlSchemeStandard::steerAll(double steering, double throttle)
{
    BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << "  steering="<<steering;

    const auto asteering = abs(steering);
    const auto &motors = m_motor_control->getMotors();
    
    // Just go straight (to avoid infinite numbers for turning radius)
    if (asteering < 0.01) {
        motors[0]->servo()->setValue(Value::fromAngleRadians(WHEEL_STRAIGHT_ANGLE));
        motors[1]->servo()->setValue(Value::fromAngleRadians(-WHEEL_STRAIGHT_ANGLE));
        motors[2]->servo()->setValue(Value::fromAngleRadians(-WHEEL_STRAIGHT_ANGLE));
        motors[3]->servo()->setValue(Value::fromAngleRadians(WHEEL_STRAIGHT_ANGLE));
        return;
    }

    // Inner angle is just the fraction of steering times max turn angle
    double inner_angle = asteering * WHEEL_MAX_TURN_ANGLE;

    // Inner wheel turning circle center from inner wheels centerpoint.
    double turning_circle_dist = tan(M_PI_2-inner_angle) * WHEEL_BASE_2;

    // Calculate outer angle
    double outer_angle = atan(WHEEL_BASE_2/(turning_circle_dist+WHEEL_BASE));

    const Value inner { Value::fromAngleRadians(WHEEL_STRAIGHT_ANGLE + inner_angle) };
    const Value outer { Value::fromAngleRadians(WHEEL_STRAIGHT_ANGLE - outer_angle) };
    if (steering > 0.0) {
        motors[0]->servo()->setValue(inner);
        motors[2]->servo()->setValue(-inner);
        motors[1]->servo()->setValue(-outer);
        motors[3]->servo()->setValue(outer);
    }
    else {
        motors[1]->servo()->setValue(-inner);
        motors[3]->servo()->setValue(inner);
        motors[0]->servo()->setValue(outer);
        motors[2]->servo()->setValue(-outer);
    }
}



};