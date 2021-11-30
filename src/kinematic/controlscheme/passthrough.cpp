#include "passthrough.h"

#include <cmath>
#include <boost/log/trivial.hpp>

#include "../../robotcontext.h"
#include "../../motor/motor.h"
#include "../../motor/motorservo.h"
#include "../../motor/motorcontrol.h"

using namespace std;

namespace Robot::Kinematic {


ControlSchemePassthrough::ControlSchemePassthrough(shared_ptr<Kinematic> kinematic) :
    AbstractControlScheme { kinematic }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}

ControlSchemePassthrough::~ControlSchemePassthrough() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void ControlSchemePassthrough::init() 
{
    const lock_guard<mutex> lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    
    m_motor_control->setPassthrough(true);

    #if 0
    m_rc_connection = m_rc_receiver->sigData.connect(
        [self_ptr=weak_from_this()] (const auto flags, const auto rssi, const auto &channels) {
            if (auto self = self_ptr.lock()) { 
                self->onRCData(flags, rssi, channels); 
            }
        }
    );
    #endif

    m_initialized = true;
}


void ControlSchemePassthrough::cleanup() 
{
    const lock_guard<mutex> lock(m_mutex);

    if (!m_initialized) 
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    for (auto &motor : m_motor_control->getMotors()) {
        motor->setEnabled(false);
        motor->servo()->setEnabled(false);
    }
    m_motor_control->setPassthrough(false);
}

/*
void ControlSchemePassthrough::onRCData(Robot::RC::Flags flags, uint8_t rssi, const Robot::RC::ChannelList &channels) {
    const lock_guard<mutex> lock(m_mutex);
    if (!m_initialized) 
        return;

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    for (auto &motor : m_motor_control->getMotors()) {
        auto servo_val = channels[SERVO_PASSTHROUGH_OFFSET+motor->servo()->getIndex()];
        auto motor_val = channels[MOTOR_PASSTHROUGH_OFFSET+motor->getIndex()];

        motor->servo()->setValue(servo_val);
        motor->setValue(motor_val);
    }
}
*/

};