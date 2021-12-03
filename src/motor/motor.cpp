#include "motor.h"

#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include <robotcontext.h>
#include "servo.h"

using namespace std;

namespace Robot::Motor {

static constexpr auto MOTOR_DEADZONE { 0.1 };

static constexpr auto PID_P { 1.0 };
static constexpr auto PID_I { 0.1 };
static constexpr auto PID_D { 0.0 };

static constexpr auto MINUTE { chrono::duration_cast<chrono::microseconds>(1min) };
static constexpr auto ENCODER_CPR { 20 };
static constexpr auto GEARING { 100 };
static constexpr auto WHEEL_CIRC_MM { 300.0 };

static constexpr auto PID_UPDATE_INTERVAL { 100ms };



Motor::Motor(uint index, recursive_mutex &mutex, const std::shared_ptr<Robot::Context> &context) :
    m_context { context },
    m_initialized { false },
    m_index { index },
    m_mutex { mutex },
    m_servo { make_unique<Servo>(index, mutex, context) },
    m_enabled { false },
    m_passthrough { false },
    m_state { FREE_SPIN },
    m_last_enc_value { 0 },
    m_odometer_base { 0 },
    m_duty { 0.0 },
    m_duty_set { 0.0 }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "[" << m_index << "]";
}



Motor::~Motor() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << "[" << m_index << "]";
}


void Motor::init() 
{
    m_last_enc_value = 0;//rc_ext_encoder_read(ENCODER_CHANNEL(m_index));
    m_odometer_base = m_last_enc_value;
    m_last_update = chrono::high_resolution_clock::now();
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
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_state = BRAKE;
    #ifdef USE_ROBOTCONTROL
    rc_motor_brake(motorChannel());
    #endif
}

void Motor::freeSpin() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
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
    const lock_guard<recursive_mutex> lock(m_mutex);
    BOOST_LOG_TRIVIAL(trace) << *this << " setDuty(" << duty << ")";
    m_state = RUNNING;
    if (m_enabled) {
        // TODO Set duty in update function
        #ifdef USE_ROBOTCONTROL
        rc_motor_set(motorChannel(), duty);
        #endif
    }
    m_duty = duty;
}

void Motor::setTargetRPM(double rpm) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    cout << m_index << *this << " setRPM(" << rpm << ")" << endl;
}


void Motor::setEnabled(bool enabled) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
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
    const lock_guard<recursive_mutex> lock(m_mutex);
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
    if (!m_enabled)
        return;

    auto now = clock::now();
    auto diff = chrono::duration_cast<chrono::microseconds>(now-m_last_update);


    // Calculate RPM
    #ifdef USE_ROBOTCONTROL
    int32_t value = rc_ext_encoder_read(encoderChannel());
    #else
    int32_t value = m_last_enc_value+ENCODER_CPR;
    //m_duty_set
    #endif

    double rpm = (double)((value-m_last_enc_value)*MINUTE.count())/((double)(ENCODER_CPR*GEARING)*diff.count());
    m_rpm = (rpm+m_rpm)/2.0;

    m_last_enc_value = value;


    // Update PID


    // Update motor duty cycle
    if (fabs(m_duty-m_duty_set)>0.01) {
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
