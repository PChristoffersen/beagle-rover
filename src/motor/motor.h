#ifndef _ROBOT_MOTOR_H_
#define _ROBOT_MOTOR_H_

#include <memory>
#include <chrono>
#include <mutex>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <robotcontrol.h>

#include <robottypes.h>
#include "types.h"

namespace Robot::Motor {

    class Motor {
        public:
            using clock = std::chrono::high_resolution_clock;
            using mutex_type = std::recursive_mutex;
            using guard = std::lock_guard<mutex_type>;

            enum State {
                RUNNING,
                FREE_SPIN,
                BRAKE
            };

            Motor(uint index, mutex_type &mutex, const std::shared_ptr<::Robot::Context> &context);
            Motor(const Motor&) = delete; // No copy constructor
            Motor(Motor&&) = delete; // No move constructor
            virtual ~Motor();

            void brake();
            void freeSpin();

            int getIndex() const { return m_index; }

            void setEnabled(bool enabled);
            bool getEnabled() const { return m_enabled; }

            bool getPassthrough() const { return m_passthrough; }


            void setValue(const Value value);
            void setDuty(double duty);
            double getDuty() const { return m_duty; }
            void setTargetRPM(double rpm);
            double getTargetRPM() const { return m_target_rpm; }

            State getState() const { return m_state; }
            double getRPM() const { return m_rpm; }

            void resetOdometer();
            double getOdometer() const;
            int32_t getEncoderValue() const { return m_last_enc_value-m_odometer_base; }

            class Servo *servo() const { return m_servo.get(); }

        protected:
            void init();
            void cleanup();

            void setPassthrough(bool passthrough);
            void update();

            friend class Control;
        private:
            std::shared_ptr<::Robot::Context> m_context;
            bool m_initialized;
            uint m_index;
            mutex_type &m_mutex;
            std::unique_ptr<class Servo> m_servo;

            bool m_enabled;
            bool m_passthrough;
            State m_state;

            std::int32_t m_last_enc_value;
            clock::time_point m_last_update;

            std::int32_t m_odometer_base;

            double m_duty;
            double m_duty_set;
            double m_target_rpm;
            double m_rpm;

            rc_filter_t m_rpm_filter;
            rc_filter_t m_pid;

            inline uint encoderChannel() const;
            inline uint motorChannel() const;

            friend std::ostream &operator<<(std::ostream &os, const Motor &self)
            {
                return os << "Motor::Motor[" << self.m_index << "]";
            }
    };

};

#endif 

