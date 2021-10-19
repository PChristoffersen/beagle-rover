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
        enum State {
            RUNNING,
            FREE_SPIN,
            BRAKE
        };


        Motor(uint8_t index);
        ~Motor();

        void init();
        void cleanup();

        void brake();
        void freeSpin();

        uint8_t getIndex() const { return m_index; }

        void setDuty(double duty);
        double getDuty() const { return m_duty; }
        void setTargetRPM(double rpm);
        double getTargetRPM() const { return m_target_rpm; }

        State getState() const { return m_state; }
        double getRPM() const { return m_rpm; }

        const boost::shared_ptr<class MotorGimbal> gimbal() const { return m_gimbal; }

    protected:
        void update();

        friend class MotorControl;
    private:
        uint8_t m_index;
        boost::shared_ptr<class MotorGimbal> m_gimbal;

        State m_state;

        int32_t m_last_enc_value;
        std::chrono::high_resolution_clock::time_point m_last_update;

        double m_duty;
        double m_target_rpm;
        double m_rpm;
        PIDController<double> m_pid;

};


#endif 

