#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "motor.h"
#include "motorgimbal.h"

using namespace std;




static inline uint8_t encoder_channel(uint8_t index) {
    return index+1;
}
static inline uint8_t motor_channel(uint8_t index) {
    return index+1;
}


Motor::Motor(uint8_t index, recursive_mutex &mutex) :
    m_initialized { false },
    m_index { index },
    m_mutex { mutex },
    m_gimbal { index, mutex },
    m_enabled { false },
    m_passthrough { false },
    m_state { FREE_SPIN },
    m_last_enc_value { 0 },
    m_odometer_base { 0 },
    m_pid { PID_P, PID_I, PID_D, bind(&Motor::getRPM, this), boost::bind(&Motor::setDuty, this, _1) }
{
    //m_pid.registerTimeFunction([]() { high_resolution_clock::now().count(); });
    m_pid.setOutputBounds(-1024.0, 1024.0);
}



Motor::~Motor() 
{
    cleanup();
}


void Motor::init() 
{
    m_last_enc_value = 0;//rc_ext_encoder_read(ENCODER_CHANNEL(m_index));
    m_odometer_base = m_last_enc_value;
    m_last_update = chrono::high_resolution_clock::now();
    m_rpm = 0;
    m_duty = 0.0;
    m_target_rpm = 0.0;

    rc_motor_free_spin(motor_channel(m_index));
    m_state = FREE_SPIN;

    m_gimbal.init();
    m_initialized = true;
}

void Motor::cleanup() 
{
    if (!m_initialized) 
        return;
    m_initialized = false;

    m_gimbal.cleanup();

    if (m_state!=FREE_SPIN) {
        rc_motor_free_spin(motor_channel(m_index));
        m_state = FREE_SPIN;
    }
}


void Motor::brake() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_state = BRAKE;
    //rc_motor_brake(MOTOR_CHANNEL(m_index));
}

void Motor::freeSpin() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_state = FREE_SPIN;
    //rc_motor_free_spin(MOTOR_CHANNEL(m_index));
}


void Motor::setDutyUS(uint32_t us) {
    setDuty(MotorControl::pulseToPos(us));
}


void Motor::setDuty(double duty) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    BOOST_LOG_TRIVIAL(info) << "Motor[" << m_index << "] setDuty(" << duty << ")";
    m_duty = duty;
    m_state = RUNNING;
    //rc_motor_set(MOTOR_CHANNEL(m_index), duty);
}

void Motor::setTargetRPM(double rpm) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    cout << m_index << " setRPM(" << rpm << ")" << endl;
}


void Motor::setEnabled(bool enabled) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_enabled = enabled;
    // TODO Disable motor
    if (m_enabled) {

    }
    else {

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
    static const auto MINUTE = chrono::duration_cast<chrono::nanoseconds>(chrono::minutes(1)).count();

    auto time = chrono::high_resolution_clock::now();
    auto diff = (time-m_last_update);

    if (diff > PID_UPDATE_INTERVAL) {
        //BOOST_LOG_TRIVIAL(info) << "RPM Update";
        int32_t value = 0;
        //int32_t value = rc_ext_encoder_read(ENCODER_CHANNEL(m_index));

        auto dur = chrono::duration_cast<chrono::nanoseconds>(diff).count();

        m_rpm = (double)((value-m_last_enc_value)*MINUTE)/((double)(ENCODER_CPR*GEARING)*dur);

        if (m_state==RUNNING) {
            // UPDATE Pid
        }

        m_last_enc_value = value;
        m_last_update = time;
    }

    m_gimbal.update();

}


