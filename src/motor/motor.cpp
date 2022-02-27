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

static constexpr auto MOTOR_DEADZONE { 0.15 };
static constexpr auto DUTY_MIN_CHANGE { 0.05 };

static constexpr auto PID_P { 0.0012f };
static constexpr auto PID_I { 0.0004f };
static constexpr auto PID_D { 0.0001f };
static constexpr auto PID_EMA_ALPHA { 0.9f };
static constexpr auto PID_INTERVAL { std::chrono::duration_cast<std::chrono::duration<float>>(MOTOR_TIMER_INTERVAL).count() };

static constexpr auto MINUTE { std::chrono::duration_cast<std::chrono::microseconds>(1min) };

static constexpr auto SIMULATED_RPM_MAX { 180.0 };

static const std::string TELEMETRY_SOURCE_NAME { "motor" };

Motor::Motor(uint index, mutex_type &mutex, const std::shared_ptr<Robot::Context> &context) :
    m_context { context },
    m_initialized { false },
    m_index { index },
    m_mutex { mutex },
    m_servo { std::make_unique<Servo>(index, mutex, context) },
    m_enabled { false },
    m_mode { Mode::FREE_SPIN },
    m_last_enc_value { 0 },
    m_odometer_base { 0 },
    m_duty { 0.0f },
    m_duty_set { 0.0f },
    m_target_rpm { 0.0f },
    m_rpm { 0.0f },
    m_pid { PID_P, PID_I, PID_D, PID_INTERVAL, PID_EMA_ALPHA },
    m_event { TELEMETRY_SOURCE_NAME + "[" + std::to_string(index) + "]", index }
{
    m_pid.setLimits(-1.0f, 1.0f);
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    m_rc_pid = RC_FILTER_INITIALIZER;
    rc_filter_pid(&m_rc_pid, PID_P, PID_I, PID_D, 8*PID_INTERVAL, PID_INTERVAL);
    rc_filter_enable_saturation(&m_rc_pid, -1.0, 1.0);
    #endif

}



Motor::~Motor() 
{
    cleanup();
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    rc_filter_free(&m_rc_pid);
    #endif
}


void Motor::init(const std::shared_ptr<::Robot::Telemetry::Telemetry> &telemetry) 
{
    Robot::Telemetry::Source::init(telemetry);

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

    m_servo->init(telemetry);
    m_initialized = true;

    sendEvent(m_event);
}

void Motor::cleanup() 
{
    if (!m_initialized) 
        return;
    m_initialized = false;

    m_servo->cleanup();

    setEnabled(false);

    Robot::Telemetry::Source::cleanup();
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

        m_event.duty = m_duty;
        m_event.rpm_target = 0.0f;
        sendEvent(m_event);
        notify(NOTIFY_DEFAULT);
    }
}

void Motor::setTargetRPM(float rpm) 
{
    const guard lock(m_mutex);
    if (m_mode!=Mode::RPM || m_target_rpm != rpm) {
        if (m_mode!=Mode::RPM) {
            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            rc_filter_reset(&m_rc_pid);
            #endif
            m_pid.reset();
            m_mode = Mode::RPM;
        }
        m_target_rpm = rpm;
        m_pid.setSetpoint(rpm);

        m_event.rpm_target = m_target_rpm;
        sendEvent(m_event);
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
        m_event.enabled = enabled;
        sendEvent(m_event);
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
    odometer_type value = (m_last_enc_value - m_odometer_base);
    return (value * WHEEL_CIRC_MM) / (GEARING * ENCODER_CPR);
}


void Motor::update() 
{
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
    int32_t value = m_last_enc_value + (m_duty/20.0*ENCODER_CPR*GEARING);
    #endif
    auto value_diff = value-m_last_enc_value;
    float rpm = (float)(value_diff*MINUTE.count())/((float)(ENCODER_CPR*GEARING)*diff.count());

    auto new_rpm = (rpm+m_rpm)/2.0;
    if (new_rpm!=m_rpm || m_last_enc_value!=value) {
        changed = true;
    }
    m_rpm = new_rpm;
    m_last_enc_value = value;

    // Update PID
    if (m_mode == Mode::RPM) {
        #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
        m_duty = std::clamp(m_duty + rc_filter_march(&m_rc_pid, m_target_rpm-m_rpm), -1.0, 1.0);
        if (m_target_rpm<0.0f && m_duty > 0.0f) {
            m_duty = 0.0f;
        }
        else if (m_target_rpm>0.0f && m_duty < 0.0f) {
            m_duty = 0.0f;
        }
        else if (m_target_rpm==0.0f) {
            m_duty = 0.0f;
        }
        #endif
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

    if (m_rpm != m_event.rpm) {
        m_event.rpm = m_rpm;
        sendEvent(m_event);
    }
    if (changed) {
        notify(NOTIFY_TELEMETRY);
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
