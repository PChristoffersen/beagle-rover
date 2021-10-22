#include <math.h>
#include <boost/log/trivial.hpp>

#include "controlbalancing.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"

using namespace boost;


shared_ptr<ControlBalancing> ControlBalancing::create(shared_ptr<class Kinematic> kinematic) {
    return shared_ptr<ControlBalancing>(new ControlBalancing(kinematic));
}


ControlBalancing::ControlBalancing(boost::shared_ptr<class Kinematic> kinematic) :
    AbstractControlScheme(kinematic)
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}

ControlBalancing::~ControlBalancing() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


void ControlBalancing::init() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    // Set all motor angles to 0 degrees and throttle to 0
    auto motor = m_motor_control->getMotor(MotorControl::FRONT_LEFT);
    auto gimbal = motor->getGimbal();
    motor->setDuty(0.0);
    motor->brake();
    gimbal->setAngle(-M_PI_2);
    gimbal->setEnabled(true);

    motor = m_motor_control->getMotor(MotorControl::FRONT_RIGHT);
    gimbal = motor->getGimbal();
    motor->setDuty(0.0);
    motor->brake();
    gimbal->setAngle(-M_PI_2);
    gimbal->setEnabled(true);
    
    motor = m_motor_control->getMotor(MotorControl::REAR_LEFT);
    gimbal = motor->getGimbal();
    motor->setDuty(0.0);
    gimbal->setAngle(M_PI_2);
    gimbal->setEnabled(true);

    motor = m_motor_control->getMotor(MotorControl::REAR_RIGHT);
    gimbal = motor->getGimbal();
    motor->setDuty(0.0);
    gimbal->setAngle(M_PI_2);
    gimbal->setEnabled(true);
}


void ControlBalancing::cleanup() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}

/*
void Kinematic::enterDriveModeSpinning() {
    m_steering_mode = STEERING_NONE;
    

}
*/
