#include "motor.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include <robotconfig.h>
#include <robotcontext.h>
#include "servo.h"

using namespace std::literals;
using namespace Robot::Config;

namespace Robot::Motor {

static constexpr auto MOTOR_DEADZONE { 0.15 };
static constexpr auto DUTY_MIN_CHANGE { 0.05 };

static constexpr auto PID_P { 0.0012 };
static constexpr auto PID_I { 0.0004 };
static constexpr auto PID_D { 0.0001 };

static constexpr auto MINUTE { std::chrono::duration_cast<std::chrono::microseconds>(1min) };

static constexpr auto SIMULATED_RPM_MAX { 180.0 };


Motor::Motor(uint index, mutex_type &mutex, const std::shared_ptr<Robot::Context> &context) :
    m_context { context },
    m_initialized { false },
    m_index { index },
    m_mutex { mutex },
    m_servo { std::make_unique<Servo>(index, mutex, context) },
    m_enabled { false },
    m_passthrough { false },
    m_state { FREE_SPIN },
    m_last_enc_value { 0 },
    m_odometer_base { 0 },
    m_duty { 0.0 },
    m_duty_set { 0.0 },
    m_target_rpm { 0.0 },
    m_rpm { 0.0 }
{
    using std::chrono::duration, std::chrono::duration_cast;

    auto updateInterval = duration_cast<duration<double>>(MOTOR_TIMER_INTERVAL).count();

    m_pid = RC_FILTER_INITIALIZER;
    rc_filter_pid(&m_pid, PID_P, PID_I, PID_D, 8*updateInterval, updateInterval);
    rc_filter_enable_saturation(&m_pid, -1.0, 1.0);

    BOOST_LOG_TRIVIAL(trace) << *this << " " << __FUNCTION__;
}



Motor::~Motor() 
{
    cleanup();
    rc_filter_free(&m_pid);
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "[" << m_index << "]";
}


void Motor::init() 
{
    m_last_enc_value = 0;//rc_ext_encoder_read(ENCODER_CHANNEL(m_index));
    m_odometer_base = m_last_enc_value;
    m_last_update = clock_type::now();
    m_rpm = 0;
    m_duty = 0.0;
    m_target_rpm = 0.0;

    #ifdef USE_ROBOTCONTROL
    rc_motor_free_spin(motorChannel());
    #endif
    m_state = FREE_SPIN;

    m_servo->init();
    m_initialized = true;
}

void Motor::cleanup() 
{
    if (!m_initialized) 
        return;
    m_initialized = false;

    m_servo->cleanup();

    setEnabled(false);
}


void Motor::brake() 
{
    const guard lock(m_mutex);
    m_state = BRAKE;
    #ifdef USE_ROBOTCONTROL
    rc_motor_brake(motorChannel());
    #endif
}

void Motor::freeSpin() 
{
    const guard lock(m_mutex);
    m_state = FREE_SPIN;
    #ifdef USE_ROBOTCONTROL
    rc_motor_free_spin(motorChannel());
    #endif
}


void Motor::setValue(const Value value) {
    setDuty(value.asPercent());
}


void Motor::setDuty(double duty) 
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << *this << " setDuty(" << duty << ")";
    m_state = RUNNING_DUTY;
    m_duty = duty;
}

void Motor::setTargetRPM(double rpm) 
{
    const guard lock(m_mutex);
    if (m_state!=RUNNING_RPM) {
        rc_filter_reset(&m_pid);
    }
    m_state = RUNNING_RPM;
    m_target_rpm = rpm;
}


void Motor::setEnabled(bool enabled) 
{
    const guard lock(m_mutex);
    if (enabled!=m_enabled) {
        m_enabled = enabled;
        BOOST_LOG_TRIVIAL(trace) << *this << " Enable " << enabled;
        if (m_enabled) {
            m_context->motorPower(true);
            m_duty_set = m_duty;
            #ifdef USE_ROBOTCONTROL
            rc_motor_set(motorChannel(), m_duty_set);
            #endif
        }
        else {
            m_duty_set = 0.0;
            #ifdef USE_ROBOTCONTROL
            rc_motor_set(motorChannel(), m_duty_set);
            #endif
            m_context->motorPower(false);
        }
    }
}

void Motor::setPassthrough(bool passthrough) 
{
    const guard lock(m_mutex);
    m_state = RUNNING_PASSTHROUGH;
    m_passthrough = passthrough;
}


void Motor::resetOdometer() 
{
    m_odometer_base = m_last_enc_value;
}


double Motor::getOdometer() const 
{
    double value = (m_last_enc_value - m_odometer_base);
    return (WHEEL_CIRC_MM * value) / (GEARING * ENCODER_CPR);
}


void Motor::update() 
{
    using std::chrono::duration_cast, std::chrono::microseconds;

    if (!m_enabled)
        return;

    auto now = clock::now();
    auto diff = duration_cast<microseconds>(now-m_last_update);


    // Calculate RPM
    #ifdef USE_ROBOTCONTROL
    int32_t value = rc_ext_encoder_read(encoderChannel());
    double rpm = (double)((value-m_last_enc_value)*MINUTE.count())/((double)(ENCODER_CPR*GEARING)*diff.count());
    #else
    int32_t value = m_last_enc_value+1;
    double rpm = m_duty_set*SIMULATED_RPM_MAX;
    #endif

    m_rpm = (rpm+m_rpm)/2.0;
    m_last_enc_value = value;

    // Update PID
    if (m_state == RUNNING_RPM) {
        m_duty = std::clamp(m_duty + rc_filter_march(&m_pid, m_target_rpm-m_rpm), -1.0, 1.0);
        if (m_target_rpm<0.0 && m_duty > 0.0) {
            m_duty = 0.0;
        }
        else if (m_target_rpm>0.0 && m_duty < 0.0) {
            m_duty = 0.0;
        }
        else if (m_target_rpm==0.0) {
            m_duty = 0.0;
        }
    }

    // Update motor duty cycle
    if (fabs(m_duty-m_duty_set)>DUTY_MIN_CHANGE) {
        //BOOST_LOG_TRIVIAL(info) << *this << " Duty " << m_duty_set << " -> " << m_duty;
        #ifdef USE_ROBOTCONTROL
        if (fabs(m_duty)<MOTOR_DEADZONE) {
            rc_motor_set(motorChannel(), 0.0);
        }
        else {
            rc_motor_set(motorChannel(), m_duty);
        }
        #endif
        m_duty_set = m_duty;
    }

    m_last_update = now;
}


inline uint Motor::encoderChannel() const {
    return m_index+1;
}

inline uint Motor::motorChannel() const {
    return m_index+1;
}


};
