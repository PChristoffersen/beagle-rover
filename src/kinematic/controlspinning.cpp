#include <boost/log/trivial.hpp>

#include "controlspinning.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"

using namespace boost;


shared_ptr<ControlSpinning> ControlSpinning::create(shared_ptr<class Kinematic> kinematic) {
    return shared_ptr<ControlSpinning>(new ControlSpinning(kinematic));
}


ControlSpinning::ControlSpinning(boost::shared_ptr<class Kinematic> kinematic) :
    AbstractControlScheme(kinematic)
{
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}

ControlSpinning::~ControlSpinning() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


void ControlSpinning::init() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    BOOST_LOG_TRIVIAL(trace) << m_motor_control;
    // Set all motor angles to 0 degrees and throttle to 0
    for (auto motor : m_motor_control->getMotors()) {
        auto gimbal = motor->getGimbal();
        
        motor->setDuty(0.0);
        motor->freeSpin();
        gimbal->setAngle(0.0);
        gimbal->setEnabled(true);
    }
}


void ControlSpinning::cleanup() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}

/*
void Kinematic::enterDriveModeSpinning() {
    m_steering_mode = STEERING_NONE;
    

}
*/
