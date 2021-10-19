#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "motor.h"
#include "motorgimbal.h"

using namespace std::chrono;
using namespace boost;
using namespace boost::asio;

#define PID_P 1.0
#define PID_I 0.1
#define PID_D 0.0


#define ENCODER_CPR 20
#define GEARING 100
#define WHEEL_CIRC_MM 285.0


#define PID_UPDATE_INTERVAL milliseconds(100)

#define ENCODER_CHANNEL(index) (index+1)
#define MOTOR_CHANNEL(index) (index+1)

Motor::Motor(uint8_t index) :
    m_index(index),
    m_gimbal(new MotorGimbal(index)),
    m_state(FREE_SPIN),
    m_pid(PID_P, PID_I, PID_D, bind(&Motor::getRPM, this), bind(&Motor::setDuty, this, _1))
{
    //m_pid.registerTimeFunction([]() { high_resolution_clock::now().count(); });
    m_pid.setOutputBounds(-1024.0, 1024.0);
}



Motor::~Motor() {

}




void Motor::init() {
    m_last_enc_value = 0;//rc_ext_encoder_read(ENCODER_CHANNEL(m_index));
    m_last_update = high_resolution_clock::now();
    m_rpm = 0;
    m_duty = 0.0;
    m_target_rpm = 0.0;

    rc_motor_free_spin(MOTOR_CHANNEL(m_index));
    m_state = FREE_SPIN;
}

void Motor::cleanup() {
    if (m_state!=FREE_SPIN) {
        rc_motor_free_spin(MOTOR_CHANNEL(m_index));
        m_state = FREE_SPIN;
    }
}


void Motor::brake() {
    m_state = BRAKE;
    rc_motor_brake(MOTOR_CHANNEL(m_index));
}

void Motor::freeSpin() {
    m_state = FREE_SPIN;
    rc_motor_free_spin(MOTOR_CHANNEL(m_index));
}


void Motor::setDuty(double duty) {
    //cout << m_index << " setDuty(" << duty << ")" << endl;
    m_duty = duty;
    m_state = RUNNING;
    rc_motor_set(MOTOR_CHANNEL(m_index), duty);
}

void Motor::setTargetRPM(double rpm) {
    std::cout << m_index << " setRPM(" << rpm << ")" << std::endl;
}


void Motor::update() {
    static const auto MINUTE = duration_cast<nanoseconds>(minutes(1)).count();

    auto time = high_resolution_clock::now();
    auto diff = (time-m_last_update);

    if (diff > PID_UPDATE_INTERVAL) {
        //BOOST_LOG_TRIVIAL(info) << "RPM Update";
        int32_t value = 0;
        //int32_t value = rc_ext_encoder_read(ENCODER_CHANNEL(m_index));

        auto dur = duration_cast<nanoseconds>(diff).count();

        m_rpm = (double)((value-m_last_enc_value)*MINUTE)/((double)(ENCODER_CPR*GEARING)*dur);

        if (m_state==RUNNING) {
            // UPDATE Pid
        }

        m_last_enc_value = value;
        m_last_update = time;
    }

    m_gimbal->update();

}


