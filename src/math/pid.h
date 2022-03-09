#ifndef _PID__H_
#define _PID__H_

#include <cinttypes>
#include <chrono>

namespace Robot::Math {

class PID {
  public:
    static constexpr auto DEFAULT_EMA_ALPHA { 1.0f };

    using sample_time_type = std::chrono::duration<float>;

    PID();
    /**
     * @brief Construct a new PID object
     * 
     * @param kp Proportional gain
     * @param ki Integral gain
     * @param kd Derivative gain
     * @param Ts Sampling time (seconds)
     * @param ema_alpha Weight factor of derivative EMA filter (0,1]
     * @param maxOutput Maximum control output magnitude
     */
    PID(float kp, float ki, float kd, sample_time_type Ts, float ema_alpha = DEFAULT_EMA_ALPHA);

    float update(float input);

    void set(float kp, float ki, float kd);
    constexpr void setKp(float kp) { m_kp = kp; }
    constexpr void setKi(float ki) { m_ki_Ts = ki * m_Ts.count(); }
    constexpr void setKd(float kd) { m_kd_Ts = kd / m_Ts.count(); }

    float getKp() const { return m_kp; }         ///< Proportional gain
    float getKi() const { return m_ki_Ts / m_Ts.count(); } ///< Integral gain
    float getKd() const { return m_kd_Ts * m_Ts.count(); } ///< Derivative gain

    void setSetpoint(float setpoint) { m_setpoint = setpoint; }
    float getSetpoint() const { return m_setpoint; }

    constexpr void setLimits(float min_output, float max_output) { 
        m_min_output = min_output; 
        m_max_output = max_output; 
    }
    float getLimitMin() const { return m_min_output; }
    float getLimitMax() const { return m_max_output; }

    /// Reset the sum of the previous errors to zero.
    void reset();

  private:
    sample_time_type m_Ts;         // Sampling time (seconds)
    float m_min_output; // Minimum control output magnitude
    float m_max_output; // Maximum control output magnitude
    float m_kp;         // Proportional gain
    float m_ki_Ts;      // Integral gain times Ts
    float m_kd_Ts;      // Derivative gain divided by Ts
    float m_ema_alpha;  // Weight factor of derivative EMA filter.
    float m_prev_input; // (Filtered) previous input for derivative.
    float m_integral;   // Sum of previous errors for integral.
    float m_setpoint;   // Target

};

}

#endif

