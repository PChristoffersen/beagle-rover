#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <memory>
#include <chrono>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <PID.h>

#include "motorcontrol.h"
#include "motorgimbal.h"

namespace Robot::Motor {

    class Motor {
        public:
            enum State {
                RUNNING,
                FREE_SPIN,
                BRAKE
            };

            Motor(int index, std::recursive_mutex &mutex);
            Motor(const Motor&) = delete; // No copy constructor
            Motor(Motor&&) = delete; // No move constructor
            virtual ~Motor();

            void brake();
            void freeSpin();

            int getIndex() const { return m_index; }

            void setEnabled(bool enabled);
            bool getEnabled() const { return m_enabled; }

            bool getPassthrough() const { return m_passthrough; }

            void setDutyUS(std::uint32_t us);
            void setDuty(double duty);
            double getDuty() const { return m_duty; }
            void setTargetRPM(double rpm);
            double getTargetRPM() const { return m_target_rpm; }

            State getState() const { return m_state; }
            double getRPM() const { return m_rpm; }

            void resetOdometer();
            double getOdometer() const;


            Gimbal *gimbal() const { return m_gimbal.get(); }

        protected:
            void init();
            void cleanup();

            void setPassthrough(bool passthrough);
            void update();

            friend class Control;
        private:
            static constexpr auto PID_P { 1.0 };
            static constexpr auto PID_I { 0.1 };
            static constexpr auto PID_D { 0.0 };

            static constexpr auto ENCODER_CPR { 20 };
            static constexpr auto GEARING { 100 };
            static constexpr auto WHEEL_CIRC_MM { 300.0 };

            static constexpr auto PID_UPDATE_INTERVAL { std::chrono::milliseconds(100) };

            bool m_initialized;
            int m_index;
            std::recursive_mutex &m_mutex;
            std::unique_ptr<Gimbal> m_gimbal;

            bool m_enabled;
            bool m_passthrough;
            State m_state;


            std::int32_t m_last_enc_value;
            std::chrono::high_resolution_clock::time_point m_last_update;

            std::int32_t m_odometer_base;

            double m_duty;
            double m_target_rpm;
            double m_rpm;
            PIDController<double> m_pid;

            int encoderChannel() const {
                return m_index+1;
            }
            int motorChannel() const {
                return m_index+1;
            }

    };

};

#endif 

