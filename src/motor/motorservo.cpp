#include "motorservo.h"

#include <cmath>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "../robotcontext.h"
#include "motorcontrol.h"

using namespace std;

namespace Robot::Motor {

static constexpr int32_t SERVO_TRIM[] { 
    0,
    0,
    0,
    0
};


Servo::Servo(uint index, recursive_mutex &mutex, const std::shared_ptr<Robot::Context> &context) :
    m_context { context },
    m_initialized { false },
    m_index { index },
    m_mutex { mutex },
    m_enabled { false },
    m_passthrough { false },
    m_limit_min { SERVO_LIMIT_MIN },
    m_limit_max { SERVO_LIMIT_MAX },
    m_trim { SERVO_TRIM[index] }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "[" << m_index << "]";
}


Servo::~Servo() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "[" << m_index << "]";
}


void Servo::init() 
{
    m_value = Value::UNSET;
    m_initialized = true;
}


void Servo::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = false;
    setEnabled(false);
}


void Servo::setEnabled(bool enabled) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (enabled!=m_enabled) {
        m_enabled = enabled;
        BOOST_LOG_TRIVIAL(info) << *this << " Enable " << enabled;
        m_context->servoPower(m_enabled);
    }
}


void Servo::setPassthrough(bool passthrough) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    //BOOST_LOG_TRIVIAL(info) << "Enable " << enable;
    m_passthrough = passthrough;
}


void Servo::setValue(const Value value)
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    //BOOST_LOG_TRIVIAL(info) << *this << " Value " << value << " ( angle=" << value.asAngle() << " )";
    m_value = value.clamp(m_limit_min, m_limit_max);
}



void Servo::setLimits(uint32_t lmin, uint32_t lmax) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    setLimitMin(lmin);
    setLimitMax(lmax);
}

void Servo::setLimitMin(uint32_t limit) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_limit_min = clamp(limit, Value::PULSE_MIN, Value::PULSE_MAX);
}

void Servo::setLimitMax(uint32_t limit) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_limit_max = clamp(limit, Value::PULSE_MIN, Value::PULSE_MAX);
}



inline uint Servo::servoChannel() const {
    return m_index+1;
}


void Servo::update() 
{
    if (m_enabled && !m_passthrough && m_value) {
        //BOOST_LOG_TRIVIAL(info) << "Pulse";
        #ifdef REAL_ROBOT
        rc_servo_send_pulse_us(servoChannel(), m_value.asServoPulse()+m_trim);
        #endif
    }
}

};