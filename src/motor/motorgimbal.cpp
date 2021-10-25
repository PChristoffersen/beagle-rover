#include <cmath>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "motorgimbal.h"


static constexpr auto PULSE_INTERVAL = std::chrono::milliseconds(20);

static constexpr uint32_t PULSE_MIN = 500;
static constexpr uint32_t PULSE_MAX = 2500;
static constexpr uint32_t PULSE_CENTER = (PULSE_MAX+PULSE_MIN)/2;
static constexpr uint32_t PULSE_RANGE = (PULSE_MAX-PULSE_MIN);

static uint8_t servo_channel(uint8_t index) {
    return index+1;
}


MotorGimbal::MotorGimbal(uint8_t index, std::recursive_mutex &mutex) :
    m_initialized(false),
    m_index(index),
    m_mutex(mutex),
    m_enabled(false),
    m_pulse_us(PULSE_CENTER),
    m_trim_us(0), 
    m_limit_min(PULSE_MIN),
    m_limit_max(PULSE_MAX)
{

}


MotorGimbal::~MotorGimbal() {
    cleanup();
}


void MotorGimbal::init() {
    m_last_pulse = std::chrono::high_resolution_clock::now();
    m_pulse_us = PULSE_CENTER;
    m_initialized = true;
}


void MotorGimbal::cleanup() {
    if (!m_initialized)
        return;
    m_initialized = false;
}


void MotorGimbal::setEnabled(bool enable) {
    const std::lock_guard<std::recursive_mutex> lock(m_mutex);
    //BOOST_LOG_TRIVIAL(info) << "Enable " << enable;
    m_enabled = enable;
}

void MotorGimbal::setPulseUS(uint32_t us) {
    const std::lock_guard<std::recursive_mutex> lock(m_mutex);
    BOOST_LOG_TRIVIAL(info) << "Pulse " << us;
    us = std::clamp(us, m_limit_min, m_limit_max);
    m_pulse_us = us;
}


void MotorGimbal::setAngle(double angle) {
    const std::lock_guard<std::recursive_mutex> lock(m_mutex);
    uint32_t pulse = angle * PULSE_RANGE / (M_PI * 2.0) + PULSE_CENTER;
    setPulseUS(pulse);
}


double MotorGimbal::getAngle() const {
    return M_PI * 2.0 * (double)((int32_t)m_pulse_us - PULSE_CENTER) / PULSE_RANGE;
}


void MotorGimbal::setTrimUS(int32_t trim) {
    const std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_trim_us = std::clamp(trim, -(int32_t)PULSE_RANGE, (int32_t)PULSE_RANGE);
}

void MotorGimbal::setLimits(uint32_t lmin, uint32_t lmax) {
    const std::lock_guard<std::recursive_mutex> lock(m_mutex);
    setLimitMin(lmin);
    setLimitMax(lmax);
}

void MotorGimbal::setLimitMin(uint32_t limit) {
    const std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_limit_min = std::clamp(limit, PULSE_MIN, PULSE_MAX);
}

void MotorGimbal::setLimitMax(uint32_t limit) {
    const std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_limit_max = std::clamp(limit, PULSE_MIN, PULSE_MAX);
}



void MotorGimbal::update() {
    auto time = std::chrono::high_resolution_clock::now();

    if (m_enabled && m_pulse_us>0) {
        auto gimbalDiff = (time-m_last_pulse);
        if (gimbalDiff > PULSE_INTERVAL) {
            //BOOST_LOG_TRIVIAL(info) << "Pulse";
            rc_servo_send_pulse_us(servo_channel(m_index), m_pulse_us+m_trim_us);
            m_last_pulse = time;
        }
    }
}
