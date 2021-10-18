#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>
#include <robotcontrol-ext.h>
#include <PID.h>


class Motor {
    public:
        Motor(uint8_t index);
        ~Motor();

        void init();
        void cleanup();

        uint8_t getIndex() const { return m_index; }

        void setDuty(double duty);
        double getRPM();
        void setRPM(double rpm);

        double getPosition() const;

    protected:
        void update();

        friend class MotorControl;
    private:
        uint8_t m_index;
        volatile shm_fbus_t *m_fbus;

        int32_t m_last_enc_value;
        std::chrono::high_resolution_clock::time_point m_last_enc_time;

        double m_duty;
        double m_target_rpm;
        double m_rpm;
        PIDController<double> m_pid;

        int32_t value;
};


#endif 

