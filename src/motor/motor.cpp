#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "motor.h"

using namespace std::chrono;
using namespace boost;
using namespace boost::asio;

#define MOTORS_MICRO
//#define MOTORS_D20

#define PID_P 1.0
#define PID_I 0.1
#define PID_D 0.0


// Micro motors
#if defined(MOTORS_MICRO)
#define ENCODER_CPR 12
#define GEARING 380

// 20D motor
#elif defined(MOTORS_20D)
#define ENCODER_CPR 20

#endif



Motor::Motor(uint8_t index) :
    m_index(index),
    m_pid(PID_P, PID_I, PID_D, bind(&Motor::getRPM, this), bind(&Motor::setDuty, this, _1))
{
    std::cout << "Init motor (" << (int)(m_index) << ")" << std::endl;
    //m_pid.registerTimeFunction([]() { high_resolution_clock::now().count(); });
    m_pid.setOutputBounds(-1024.0, 1024.0);

}



Motor::~Motor() {

}




void Motor::init() {
    m_fbus = rc_ext_fbus_get_shm();
    m_last_enc_value = 0;//rc_ext_encoder_read(m_index+1);
    m_last_enc_time = high_resolution_clock::now();
    m_rpm = 0;
    m_duty = 0.0;
    m_target_rpm = 100.0;
    value = 0;
}

void Motor::cleanup() {
}


void Motor::setDuty(double duty) {
    //cout << m_index << " setDuty(" << duty << ")" << endl;
    m_duty = duty;
}

double Motor::getRPM() {
    return m_rpm;
}

void Motor::setRPM(double rpm) {
    std::cout << m_index << " setRPM(" << rpm << ")" << std::endl;
}


double Motor::getPosition() const {
    if (m_fbus) {
        return m_fbus->channels[m_index];
    }
    return 0.0;
}


void Motor::update() {
    static const auto MINUTE = duration_cast<nanoseconds>(minutes(1)).count();
    //auto value = m_last_enc_value+10*m_duty; //0;//rc_ext_encoder_read(m_index+1);
    auto time = high_resolution_clock::now();

    auto diff = (value-m_last_enc_value);
    auto dur = duration_cast<nanoseconds>(time - m_last_enc_time).count();

    if (dur > 0) {
        m_rpm = (double)(diff*MINUTE)/((double)ENCODER_CPR*GEARING*dur);
        m_pid.tick();

        std::cout << format("%.3f") % m_duty 
            << "  Diff: " << format("%.1f") % diff 
            << "  " << "dur " << dur << " ns    " 
            << format("%.3f") % m_rpm << " rpm   " 
            << std::endl;
    }

    m_last_enc_value = value;
    m_last_enc_time = time;
}

