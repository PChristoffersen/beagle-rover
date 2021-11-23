#ifndef _MOTOR_H_
#define _MOTOR_H_

#include <memory>
#include <chrono>
#include <mutex>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include "motorcontrol.h"
#include "motorgimbal.h"
#include "../robottypes.h"

namespace Robot::Motor {

    class Motor {
        public:
            enum State {
                RUNNING,
                FREE_SPIN,
                BRAKE
            };

            Motor(uint index, std::recursive_mutex &mutex, const std::shared_ptr<Robot::Context> &context);
            Motor(const Motor&) = delete; // No copy constructor
            Motor(Motor&&) = delete; // No move constructor
            virtual ~Motor();

            void brake();
            void freeSpin();

            int getIndex() const { return m_index; }

            void setEnabled(bool enabled);
            bool getEnabled() const { return m_enabled; }

            bool getPassthrough() const { return m_passthrough; }


            void setValue(const Robot::InputValue value);
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
            std::shared_ptr<Robot::Context> m_context;
            bool m_initialized;
            uint m_index;
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

            inline uint encoderChannel() const;
            inline uint motorChannel() const;

    };

};

#endif 

