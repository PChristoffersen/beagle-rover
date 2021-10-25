#include <boost/log/trivial.hpp>

#include "controlspinning.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"



ControlSpinning::ControlSpinning(std::shared_ptr<class Kinematic> kinematic) :
    AbstractControlScheme(kinematic)
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}

ControlSpinning::~ControlSpinning() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


void ControlSpinning::init() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    #if 0
    // Set all motor angles to 0 degrees and throttle to 0
    for (auto &motor : m_motor_control->getMotors()) {
        motor->setDuty(0.0);
        motor->freeSpin();
        motor->gimbal().setAngle(0.0);
        motor->gimbal().setEnabled(true);
    }
    #endif
}


void ControlSpinning::cleanup() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}

/*
void Kinematic::enterDriveModeSpinning() {
    m_steering_mode = STEERING_NONE;
    

}
*/
