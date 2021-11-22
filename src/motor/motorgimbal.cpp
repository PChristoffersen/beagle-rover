#include "motorgimbal.h"

#include <cmath>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "motorcontrol.h"

using namespace std;

namespace Robot::Motor {

static constexpr auto PULSE_INTERVAL { 20ms };
static constexpr auto GIMBAL_LIMIT_MIN { 650u };
static constexpr auto GIMBAL_LIMIT_MAX { 2350u };
static constexpr int32_t GIMBAL_TRIM[] { 
    0,
    0,
    0,
    0
};


Gimbal::Gimbal(uint index, recursive_mutex &mutex, const std::shared_ptr<Robot::Context> &context) :
    m_context { context },
    m_initialized { false },
    m_index { index },
    m_mutex { mutex },
    m_enabled { false },
    m_passthrough { false },
    m_limit_min { GIMBAL_LIMIT_MIN },
    m_limit_max { GIMBAL_LIMIT_MAX },
    m_trim { GIMBAL_TRIM[index] }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "[" << m_index << "]";
}


Gimbal::~Gimbal() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "[" << m_index << "]";
}


void Gimbal::init() 
{
    m_last_pulse = chrono::high_resolution_clock::now();
    m_value.unset();
    m_initialized = true;
}


void Gimbal::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = false;
    setEnabled(false);
}


void Gimbal::setEnabled(bool enabled) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (enabled!=m_enabled) {
        m_enabled = enabled;
        BOOST_LOG_TRIVIAL(info) << "Gimbal[" << m_index << "] Enable " << enabled;
        m_context->servoPower(m_enabled);
    }
}


void Gimbal::setPassthrough(bool passthrough) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    //BOOST_LOG_TRIVIAL(info) << "Enable " << enable;
    m_passthrough = passthrough;
}


void Gimbal::setValue(const Robot::InputValue value)
{
    BOOST_LOG_TRIVIAL(info) << "Gimbal[" << m_index << "] Value " << value;
    m_value = value;
}

void Gimbal::setPulseUS(uint32_t us) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    BOOST_LOG_TRIVIAL(info) << "Gimbal[" << m_index << "] Pulse " << us << "us";
    m_value = Robot::InputValue::fromMicroSeconds(us).clamp(m_limit_min, m_limit_max);
}


void Gimbal::setAngle(double angle) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_value = Robot::InputValue::fromAngle(angle).clamp(m_limit_min, m_limit_max);
}


double Gimbal::getAngle() const 
{
    return m_value.asAngle();
}


void Gimbal::setAngleRadians(double angle) {
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_value = Robot::InputValue::fromAngleRadians(angle).clamp(m_limit_min, m_limit_max);
}

double Gimbal::getAngleRadians() const {
    return m_value.asAngleRadians();
}



void Gimbal::setLimits(uint32_t lmin, uint32_t lmax) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    setLimitMin(lmin);
    setLimitMax(lmax);
}

void Gimbal::setLimitMin(uint32_t limit) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_limit_min = clamp(limit, Robot::InputValue::PULSE_MIN, Robot::InputValue::PULSE_MAX);
}

void Gimbal::setLimitMax(uint32_t limit) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_limit_max = clamp(limit, Robot::InputValue::PULSE_MIN, Robot::InputValue::PULSE_MAX);
}



inline uint Gimbal::servoChannel() const {
    return m_index+1;
}


void Gimbal::update() 
{
    if (m_enabled && !m_passthrough && m_value) {
        auto time = chrono::high_resolution_clock::now();
        auto gimbalDiff = (time-m_last_pulse);
        if (gimbalDiff > PULSE_INTERVAL) {
            //BOOST_LOG_TRIVIAL(info) << "Pulse";
            rc_servo_send_pulse_us(servoChannel(), m_value.asServoPulse()+m_trim);
            m_last_pulse = time;
        }
    }
}

};