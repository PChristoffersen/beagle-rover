 #include "pid.h"

#include <cmath>
#include <limits>

namespace Robot::Math {

PID::PID(float kp, float ki, float kd, float Ts, float ema_alpha) : 
    m_Ts(Ts), 
    m_kp { 0.0f },
    m_ki_Ts { 0.0f },
    m_kd_Ts { 0.0f },
    m_ema_alpha { ema_alpha },
    m_prev_input { 0.0f },
    m_integral { 0.0f },
    m_setpoint { 0.0f },
    m_min_output { std::numeric_limits<float>::min() },
    m_max_output { std::numeric_limits<float>::max() }
{
    setKp(kp);
    setKi(ki);
    setKd(kd);
}


void PID::set(float kp, float ki, float kd)
{
    setKp(kp);
    setKi(ki);
    setKd(kd);
    reset();
}


void PID::reset() 
{ 
    m_integral = 0.0f; 
}


float PID::update(float input) 
{
    // Compute difference between input and target point
    float error = m_setpoint - input;
    // The integral - sum of current and previous errors
    float new_integral = m_integral + error;
    // Compute the difference between the current and the previous input,
    // but compute a weighted average using a factor α ∊ (0,1]
    float diff = m_ema_alpha * (m_prev_input - input);
    // Update the average
    m_prev_input -= diff;

    // Compute PID output
    float output = m_kp * error + m_ki_Ts * m_integral + m_kd_Ts * diff;

    // Clamp output, and only set integral if we are not already saturated
    if (output > m_max_output) {
        output = m_max_output;
    }
    else if (output < m_min_output) {
        output = -m_min_output;
    }
    else {
        m_integral = new_integral;
    }

    return output;
}

};