#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <stdint.h>
#include <chrono>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <robotcontrol-ext.h>
#include <PID.h>



class Motor : public boost::enable_shared_from_this<Motor> {
    public:
        enum State {
            RUNNING,
            FREE_SPIN,
            BRAKE
        };

        [[nodiscard]] static boost::shared_ptr<Motor> create(uint8_t index, boost::shared_ptr<std::recursive_mutex> mutex);

        virtual ~Motor();

        void brake();
        void freeSpin();

        uint8_t getIndex() const { return m_index; }

        void setDuty(double duty);
        double getDuty() const { return m_duty; }
        void setTargetRPM(double rpm);
        double getTargetRPM() const { return m_target_rpm; }

        State getState() const { return m_state; }
        double getRPM() const { return m_rpm; }

        void resetOdometer();
        double getOdometer() const;

        void setEnabled(bool enabled);
        bool getEnabled() const { return m_enabled; }

        const boost::shared_ptr<class MotorGimbal> getGimbal() const { return m_gimbal; }

    protected:
        void init();
        void cleanup();

        void update();

        friend class MotorControl;
    private:
        uint8_t m_index;
        boost::shared_ptr<std::recursive_mutex> m_mutex;
        boost::shared_ptr<class MotorGimbal> m_gimbal;

        bool m_enabled;
        State m_state;

        int32_t m_last_enc_value;
        std::chrono::high_resolution_clock::time_point m_last_update;

        int32_t m_odometer_base;

        double m_duty;
        double m_target_rpm;
        double m_rpm;
        PIDController<double> m_pid;

        Motor(uint8_t index, boost::shared_ptr<std::recursive_mutex> mutex);
};


#endif 

