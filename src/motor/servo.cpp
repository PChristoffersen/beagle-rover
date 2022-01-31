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


Servo::Servo(uint index, mutex_type &mutex, const std::shared_ptr<Robot::Context> &context) :
    m_context { context },
    m_initialized { false },
    m_index { index },
    m_mutex { mutex },
    m_enabled { false },
    m_limit_min { WHEEL_SERVO_LIMIT_MIN },
    m_limit_max { WHEEL_SERVO_LIMIT_MAX },
    m_trim { SERVO_TRIM[index] }, 
    m_event { TELEMETRY_SOURCE_NAME + "[" + std::to_string(index) + "]", index }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "[" << m_index << "]";
}


Servo::~Servo() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "[" << m_index << "]";
}


void Servo::init(const std::shared_ptr<Robot::Telemetry::Telemetry> &telemetry) 
{
    Robot:Telemetry::Source::init(telemetry);

    m_value = Value::CENTER;
    m_initialized = true;

    m_event.angle = m_value.asAngle();
    sendEvent(m_event);
}


void Servo::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = false;
    setEnabled(false);

    Robot:Telemetry::Source::cleanup();
}


void Servo::setEnabled(bool enabled) 
{
    const guard lock(m_mutex);
    if (enabled!=m_enabled) {
        m_enabled = enabled;
        BOOST_LOG_TRIVIAL(trace) << *this << " Enable " << enabled;
        m_context->servoPower(m_enabled);

        m_event.enabled = m_enabled;
        sendEvent(m_event);
        notify(nullptr);
    }
}


void Servo::setValue(const Value value)
{
    const guard lock(m_mutex);
    //BOOST_LOG_TRIVIAL(info) << *this << " Value " << value << " ( angle=" << value.asAngleDegrees() << " )";
    auto v = value.clamp(m_limit_min, m_limit_max);
    if (m_value != v) {
        m_event.angle = m_value.asAngle();
        sendEvent(m_event);
        notify(nullptr);
    }
}



void Servo::setLimits(uint32_t lmin, uint32_t lmax) 
{
    const guard lock(m_mutex);
    m_limit_min = std::clamp(lmin, Value::PULSE_MIN, Value::PULSE_MAX);
    m_limit_max = std::clamp(lmax, Value::PULSE_MIN, Value::PULSE_MAX);
    notify(nullptr);
}

void Servo::setLimitMin(uint32_t limit) 
{
    const guard lock(m_mutex);
    m_limit_min = std::clamp(limit, Value::PULSE_MIN, Value::PULSE_MAX);
    notify(nullptr);
}

void Servo::setLimitMax(uint32_t limit) 
{
    const guard lock(m_mutex);
    m_limit_max = std::clamp(limit, Value::PULSE_MIN, Value::PULSE_MAX);
    notify(nullptr);
}



inline uint Servo::servoChannel() const {
    return m_index+1;
}


void Servo::update() 
{
    if (m_enabled && m_value) {
        //BOOST_LOG_TRIVIAL(info) << "Pulse";
        #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
        rc_servo_send_pulse_us(servoChannel(), m_value.asServoPulse()+m_trim);
        #endif
    }
}

};