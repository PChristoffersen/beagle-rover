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


Gimbal::Gimbal(int index, recursive_mutex &mutex) :
    m_initialized { false },
    m_index { index },
    m_mutex { mutex },
    m_enabled { false },
    m_passthrough { false },
    m_pulse_us { PULSE_CENTER },
    m_trim_us { 0 }, 
    m_limit_min { PULSE_MIN },
    m_limit_max { PULSE_MAX }
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
    m_pulse_us = PULSE_CENTER;
    m_initialized = true;
}


void Gimbal::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = false;
}


void Gimbal::setEnabled(bool enable) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    //BOOST_LOG_TRIVIAL(info) << "Enable " << enable;
    m_enabled = enable;
}


void Gimbal::setPassthrough(bool passthrough) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    //BOOST_LOG_TRIVIAL(info) << "Enable " << enable;
    m_passthrough = passthrough;
}



void Gimbal::setPulseUS(uint32_t us) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    //BOOST_LOG_TRIVIAL(info) << "Gimbal[" << m_index << "] setPulseUS(" << us << ")";
    us = clamp(us, m_limit_min, m_limit_max);
    m_pulse_us = us;
}


void Gimbal::setAngle(double angle) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    uint32_t pulse = angle * PULSE_RANGE / (M_PI * 2.0) + PULSE_CENTER;
    setPulseUS(pulse);
}


double Gimbal::getAngle() const 
{
    return M_PI * 2.0 * (double)((int32_t)m_pulse_us - PULSE_CENTER) / PULSE_RANGE;
}


void Gimbal::setAngleDegrees(double angle) {
    setAngle( angle * M_PI / 180.0 );
}

double Gimbal::getAngleDegrees() const {
    return getAngle() * 180.0 / M_PI;
}



void Gimbal::setTrimUS(int32_t trim) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_trim_us = clamp(trim, -(int32_t)PULSE_RANGE, (int32_t)PULSE_RANGE);
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
    m_limit_min = clamp(limit, PULSE_MIN, PULSE_MAX);
}

void Gimbal::setLimitMax(uint32_t limit) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_limit_max = clamp(limit, PULSE_MIN, PULSE_MAX);
}



void Gimbal::update() 
{
    if (m_enabled && !m_passthrough && m_pulse_us>0) {
        auto time = chrono::high_resolution_clock::now();
        auto gimbalDiff = (time-m_last_pulse);
        if (gimbalDiff > PULSE_INTERVAL) {
            //BOOST_LOG_TRIVIAL(info) << "Pulse";
            //rc_servo_send_pulse_us(servoChannel(m_index), m_pulse_us+m_trim_us);
            m_last_pulse = time;
        }
    }
}

};