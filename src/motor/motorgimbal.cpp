#include <math.h>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "motorgimbal.h"

using namespace std::chrono;
using namespace boost;


#define PULSE_INTERVAL milliseconds(20)
#define SERVO_CHANNEL(index) (index+1)

#define PULSE_MIN  500
#define PULSE_MAX 2500
#define PULSE_CENTER ((PULSE_MAX+PULSE_MIN)/2)
#define PULSE_RANGE (PULSE_MAX-PULSE_MIN)


shared_ptr<MotorGimbal> MotorGimbal::create(uint8_t index, shared_ptr<std::recursive_mutex> mutex) {
    return shared_ptr<MotorGimbal>(new MotorGimbal(index, mutex));
}


MotorGimbal::MotorGimbal(uint8_t index, shared_ptr<std::recursive_mutex> mutex) :
    m_index(index),
    m_mutex(mutex),
    m_enabled(false),
    m_pulse_us(PULSE_CENTER)
{

}


MotorGimbal::~MotorGimbal() {

}


void MotorGimbal::init() {
    m_last_pulse = high_resolution_clock::now();
    m_pulse_us = PULSE_CENTER;
}


void MotorGimbal::cleanup() {
}


void MotorGimbal::setEnabled(bool enable) {
    //BOOST_LOG_TRIVIAL(info) << "Enable " << enable;
    m_enabled = enable;
}

void MotorGimbal::setPulseUS(uint32_t us) {
    const std::lock_guard<std::recursive_mutex> lock(*m_mutex);
    //BOOST_LOG_TRIVIAL(info) << "Pulse " << us;
    m_pulse_us = us;
}


void MotorGimbal::setAngle(double angle) {
    const std::lock_guard<std::recursive_mutex> lock(*m_mutex);
    //m_pulse_us;
    
}


double MotorGimbal::getAngle() const {
    if (m_enabled) {
        return M_PI * 2.0 * (double)(m_pulse_us - PULSE_CENTER) / PULSE_RANGE;
    }
    return 0.0;
}


void MotorGimbal::update() {
    auto time = high_resolution_clock::now();

    if (m_enabled && m_pulse_us>0) {
        auto gimbalDiff = (time-m_last_pulse);
        if (gimbalDiff > PULSE_INTERVAL) {
            //BOOST_LOG_TRIVIAL(info) << "Pulse";
            rc_servo_send_pulse_us(SERVO_CHANNEL(m_index), m_pulse_us);
            m_last_pulse = time;
        }
    }
}
