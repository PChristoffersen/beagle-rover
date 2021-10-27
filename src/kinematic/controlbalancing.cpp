#include <math.h>
#include <boost/log/trivial.hpp>

#include "controlbalancing.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"

using namespace std;


ControlBalancing::ControlBalancing(shared_ptr<class Kinematic> kinematic) :
    AbstractControlScheme { kinematic }
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


ControlBalancing::~ControlBalancing() 
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


void ControlBalancing::init() 
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

#if 0
    auto &motors = m_motor_control->getMotors();

    // Set all motor angles to 0 degrees and throttle to 0
    auto motor = m_motor_control->getMotor(MotorControl::FRONT_LEFT);
    motor->setDuty(0.0);
    motor->brake();
    motor->gimbal().setAngle(-M_PI_2);
    motor->gimbal().setEnabled(true);

    motor = m_motor_control->getMotor(MotorControl::FRONT_RIGHT);
    motor->setDuty(0.0);
    motor->brake();
    motor->gimbal().setAngle(-M_PI_2);
    motor->gimbal().setEnabled(true);

    motor = m_motor_control->getMotor(MotorControl::REAR_LEFT);
    motor->setDuty(0.0);
    motor->gimbal().setAngle(M_PI_2);
    motor->gimbal().setEnabled(true);

    motor = m_motor_control->getMotor(MotorControl::REAR_RIGHT);
    motor->setDuty(0.0);
    motor->gimbal().setAngle(M_PI_2);
    motor->gimbal().setEnabled(true);
    #endif
}


void ControlBalancing::cleanup() 
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}

/*
void Kinematic::enterDriveModeSpinning() {
    m_steering_mode = STEERING_NONE;
    

}
*/
