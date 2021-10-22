
#include "controlnormal.h"
#include <boost/log/trivial.hpp>
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"

using namespace boost;


shared_ptr<ControlNormal> ControlNormal::create(shared_ptr<class Kinematic> kinematic) {
    return shared_ptr<ControlNormal>(new ControlNormal(kinematic));
}


ControlNormal::ControlNormal(boost::shared_ptr<class Kinematic> kinematic) :
    AbstractControlScheme(kinematic)
{
    BOOST_LOG_TRIVIAL(trace) << "Normal()";
}

ControlNormal::~ControlNormal() {
    BOOST_LOG_TRIVIAL(trace) << "~Normal()";
}


void ControlNormal::init() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}


void ControlNormal::cleanup() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
}
