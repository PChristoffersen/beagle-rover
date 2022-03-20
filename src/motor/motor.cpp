#include "motor.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <robotcontext.h>
#include "servo.h"

using namespace std::literals;
using namespace Robot::Config;

namespace Robot::Motor {

static constexpr auto MOTOR_DEADZONE { 0.05 };
static constexpr auto DUTY_MIN_CHANGE { 0.02 };

static constexpr auto PID_P { 0.0012f };
static constexpr auto PID_I { 0.0004f };
static constexpr auto PID_D { 0.0001f };
static constexpr auto PID_EMA_ALPHA { 0.9f };
static constexpr Robot::Math::PID::sample_time_type PID_INTERVAL { MOTOR_TIMER_INTERVAL };

static constexpr auto MINUTE { std::chrono::duration_cast<std::chrono::microseconds>(1min) };

static constexpr auto SIMULATED_RPM_MAX { 180.0 };

Motor::Motor(uint index, const std::shared_ptr<Robot::Context> &context, const strand_type &strand, class Servo *servo) :
    WithStrand { strand },
    m_context { context },
    m_initialized { false },
    m_index { index },
    m_servo { servo },
    m_enabled { false },
    m_mode { Mode::FREE_SPIN },
    m_last_enc_value { 0 },
    m_odometer_base { 0 },
    m_duty { 0.0f },
    m_duty_set { 0.0f },
    m_target_rpm { 0.0f },
    m_rpm { 0.0f },
    m_pid { PID_P, PID_I, PID_D, PID_INTERVAL, PID_EMA_ALPHA }
{
    m_pid.setLimits(-1.0f, 1.0f);
}



Motor::~Motor() 
{
    cleanup();
}


void Motor::init() 
{
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    rc_ext_encoder_read(encoderChannel());
    #else
    m_last_enc_value = 0;
    #endif
    m_odometer_base = m_last_enc_value;
    m_last_update = clock_type::now();
    m_rpm = 0;
    m_duty = 0.0;
    m_target_rpm = 0.0;

    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    rc_motor_free_spin(motorChannel());
    #endif
    m_mode = Mode::FREE_SPIN;

    m_initialized = true;
}

void Motor::cleanup() 
{
    if (!m_initialized) 
        return;
    m_initialized = false;

    setEnabled(false);
}


void Motor::brake() 
{
    const guard lock(m_mutex);
    if (m_mode != Mode::BRAKE) {
        m_mode = Mode::BRAKE;
        #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
        rc_motor_brake(motorChannel());
        #endif
        notify(NOTIFY_DEFAULT);
    }
}

void Motor::freeSpin() 
{
    const guard lock(m_mutex);
    if (m_mode != Mode::FREE_SPIN) {
        m_mode = Mode::FREE_SPIN;
        #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
        rc_motor_free_spin(motorChannel());
        #endif
        notify(NOTIFY_DEFAULT);
    }
}


void Motor::setValue(const Value value) {
    setDuty(value.asFloat());
}


void Motor::setDuty(float duty) 
{
    const guard lock(m_mutex);
    if (duty != m_duty || m_mode != Mode::DUTY) {
        BOOST_LOG_TRIVIAL(trace) << *this << " setDuty(" << duty << ")";
        m_mode = Mode::DUTY;
        m_duty = duty;
        notify(NOTIFY_DEFAULT);
    }
}

void Motor::setTargetRPM(float rpm) 
{
    const guard lock(m_mutex);
    if (m_mode!=Mode::RPM || m_target_rpm != rpm) {
        if (m_mode!=Mode::RPM) {
            m_pid.reset();
            m_mode = Mode::RPM;
        }
        m_target_rpm = rpm;
        m_pid.setSetpoint(rpm);
        notify(NOTIFY_DEFAULT);
    }
}


void Motor::setEnabled(bool enabled) 
{
    const guard lock(m_mutex);
    if (enabled!=m_enabled) {
        m_enabled = enabled;
        BOOST_LOG_TRIVIAL(trace) << *this << " Enable " << enabled;
        m_context->motorPower(m_enabled);
        if (m_enabled) {
            m_duty_set = m_duty;
            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            rc_motor_set(motorChannel(), m_duty_set);
            #endif
        }
        else {
            m_duty_set = 0.0;
            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            rc_motor_set(motorChannel(), m_duty_set);
            #endif
        }
        notify(NOTIFY_DEFAULT);
    }
}

void Motor::resetOdometer() 
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(info) << *this << " resetOdometer";
    m_odometer_base = m_last_enc_value;
    notify(NOTIFY_TELEMETRY);
}


Motor::odometer_type Motor::getOdometer() const 
{
    const guard lock(m_mutex);
    odometer_type value = (m_last_enc_value - m_odometer_base);
    return (value * WHEEL_CIRC_MM) / (WHEEL_GEARING * WHEEL_ENCODER_CPR);
}


void Motor::update() 
{
    const guard lock(m_mutex);
    using std::chrono::duration_cast, std::chrono::microseconds;

    if (!m_enabled)
        return;

    bool changed = false;
    auto now = clock_type::now();
    auto diff = duration_cast<microseconds>(now-m_last_update);


    // Calculate RPM
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    int32_t value = rc_ext_encoder_read(encoderChannel());
    #else
    int32_t value = m_last_enc_value + (m_duty/20.0*WHEEL_ENCODER_CPR*WHEEL_GEARING);
    #endif
    auto value_diff = value-m_last_enc_value;
    float rpm = (float)(value_diff*MINUTE.count())/((float)(WHEEL_ENCODER_CPR*WHEEL_GEARING)*diff.count());

    auto new_rpm = (rpm+m_rpm)/2.0;
    if (new_rpm!=m_rpm || m_last_enc_value!=value) {
        changed = true;
    }
    m_rpm = new_rpm;
    m_last_enc_value = value;

    // Update PID
    if (m_mode == Mode::RPM) {
        m_duty = m_pid.update(m_rpm);
    }

    // Update motor duty cycle
    if (fabs(m_duty-m_duty_set)>DUTY_MIN_CHANGE) {
        BOOST_LOG_TRIVIAL(trace) << *this << " Duty " << m_duty_set << " -> " << m_duty;
        #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
        if (fabs(m_duty)<MOTOR_DEADZONE) {
            rc_motor_set(motorChannel(), 0.0);
        }
        else {
            rc_motor_set(motorChannel(), m_duty);
        }
        #endif
        m_duty_set = m_duty;
    }

    if (changed) {
        defer([this]{ notify(NOTIFY_TELEMETRY); });
    }

    m_last_update = now;
}




inline uint Motor::encoderChannel() const {
    return m_index+1;
}

inline uint Motor::motorChannel() const {
    return m_index+1;
}


}
