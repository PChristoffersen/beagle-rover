#include <math.h>
#include <boost/log/trivial.hpp>

#include "controlschemepassthrough.h"
#include "../robotcontext.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"

using namespace std;


ControlSchemePassthrough::ControlSchemePassthrough(shared_ptr<class Kinematic> kinematic) :
    AbstractControlScheme { kinematic }
{
    BOOST_LOG_TRIVIAL(trace) << "Passthrough()";
}

ControlSchemePassthrough::~ControlSchemePassthrough() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << "~Passthrough()";
}


void ControlSchemePassthrough::init() 
{
    const lock_guard<mutex> lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    
    m_rc_connection = m_rc_receiver->sigData.connect(
        [self_ptr=weak_from_this()] (uint8_t flags, uint8_t rssi, const auto &channels) {
            if (auto self = self_ptr.lock()) { 
                self->onRCData(flags, rssi, channels); 
            }
        }
    );

    m_initialized = true;
}


void ControlSchemePassthrough::cleanup() 
{
    const lock_guard<mutex> lock(m_mutex);

    if (!m_initialized) 
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    m_rc_connection.disconnect();

    for (auto &motor : m_motor_control->getMotors()) {
        motor->setEnabled(false);
        motor->gimbal().setEnabled(false);
    }
    m_motor_control->setPassthrough(false);
}


void ControlSchemePassthrough::onRCData(uint8_t flags, uint8_t rssi, const RCReceiver::ChannelList &channels) {
    const lock_guard<mutex> lock(m_mutex);
    if (!m_initialized) 
        return;

    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;

    for (auto &motor : m_motor_control->getMotors()) {
        auto &gimbal { motor->gimbal() };
        uint32_t servo_val = channels[MotorControl::SERVO_PASSTHROUGH_OFFSET+gimbal.getIndex()];
        uint32_t motor_val = channels[MotorControl::MOTOR_PASSTHROUGH_OFFSET+motor->getIndex()];

        gimbal.setPulseUS(servo_val);
        motor->setDutyUS(motor_val);
    }
}
