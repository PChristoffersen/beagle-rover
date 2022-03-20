#include "servo.h"

#include <cmath>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <robotcontext.h>
#include "control.h"

using namespace std::literals;
using namespace Robot::Config;

namespace Robot::Motor {

static constexpr int32_t SERVO_TRIM[] { 
    0,
    0,
    0,
    0
};

static const std::string TELEMETRY_SOURCE_NAME { "servo" };


Servo::Servo(uint index, const std::shared_ptr<Robot::Context> &context, const strand_type &strand) :
    WithStrand { strand },
    m_context { context },
    m_initialized { false },
    m_index { index },
    m_enabled { false },
    m_limit_min { WHEEL_SERVO_LIMIT_MIN },
    m_limit_max { WHEEL_SERVO_LIMIT_MAX },
    m_trim { SERVO_TRIM[index] }
{
}


Servo::~Servo() 
{
    cleanup();
}


void Servo::init() 
{
    m_value = Value::CENTER;
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
    const guard lock(m_mutex);
    if (enabled!=m_enabled) {
        m_enabled = enabled;
        BOOST_LOG_TRIVIAL(trace) << *this << " Enable " << enabled;
        m_context->servoPower(m_enabled);
    }
}


void Servo::setValue(const Value value)
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << *this << " Value " << value << " ( angle=" << value.asAngleDegrees() << " )";
    auto v = value.clamp(m_limit_min, m_limit_max);
    if (m_value != v) {
        m_value = v;
        notify(NOTIFY_TELEMETRY);
    }
}



void Servo::setLimits(Value lmin, Value lmax) 
{
    const guard lock(m_mutex);
    m_limit_min = lmin;
    m_limit_max = lmax;
    notify(NOTIFY_DEFAULT);
}

void Servo::setLimitMin(Value limit) 
{
    const guard lock(m_mutex);
    m_limit_min = limit;
    notify(NOTIFY_DEFAULT);
}

void Servo::setLimitMax(Value limit) 
{
    const guard lock(m_mutex);
    m_limit_max = limit;
    notify(NOTIFY_DEFAULT);
}



inline uint Servo::servoChannel() const {
    return m_index+1;
}


void Servo::update() 
{
    if (m_enabled) {
        //BOOST_LOG_TRIVIAL(info) << "Pulse";
        #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
        rc_servo_send_pulse_us(servoChannel(), m_value.asServoPulse()+m_trim);
        #endif
    }
}

}