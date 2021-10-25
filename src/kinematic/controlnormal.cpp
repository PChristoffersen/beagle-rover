#include <math.h>
#include <boost/log/trivial.hpp>

#include "controlnormal.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"


ControlNormal::ControlNormal(std::shared_ptr<class Kinematic> kinematic) :
    AbstractControlScheme(kinematic)
{
    BOOST_LOG_TRIVIAL(trace) << "Normal()";
}

ControlNormal::~ControlNormal() {
    BOOST_LOG_TRIVIAL(trace) << "~Normal()";
}


void ControlNormal::init() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    
    #if 0
    for (auto &motor : m_motor_control->getMotors()) {
        motor->setDuty(0.0);
        motor->gimbal().setAngle(M_PI_2);
        motor->gimbal().setEnabled(true);
    }
    #endif
}


void ControlNormal::cleanup() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}
