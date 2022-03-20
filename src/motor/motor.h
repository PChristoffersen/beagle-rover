#ifndef _ROBOT_MOTOR_H_
#define _ROBOT_MOTOR_H_

#include <memory>
#include <chrono>
#include <mutex>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <robotconfig.h>
#include <robottypes.h>
#include <math/pid.h>
#include <common/withstrand.h>
#include <common/withmutex.h>
#include <common/withnotify.h>
#include "types.h"

namespace Robot::Motor {

    class Motor : public WithMutexStd, public WithNotifyInt, public WithStrand {
        public:
            static constexpr notify_type NOTIFY_TELEMETRY { 1 };

            using clock_type = std::chrono::high_resolution_clock;

            using odometer_type = std::int32_t;
            using encoder_type = std::int32_t;

            enum class Mode {
                DUTY,
                RPM,
                FREE_SPIN,
                BRAKE
            };

            Motor(uint index, const std::shared_ptr<::Robot::Context> &context, const strand_type &strand, class Servo *servo);
            Motor(const Motor&) = delete; // No copy constructor
            Motor(Motor&&) = delete; // No move constructor
            virtual ~Motor();

            void brake();
            void freeSpin();

            int getIndex() const { return m_index; }

            void setEnabled(bool enabled);
            bool getEnabled() const { return m_enabled; }

            void setValue(const Value value);
            void setDuty(float duty);
            float getDuty() const { return m_duty; }
            void setTargetRPM(float rpm);
            float getTargetRPM() const { return m_target_rpm; }

            Mode getMode() const { return m_mode; }
            float getRPM() const { return m_rpm; }

            void resetOdometer();
            odometer_type getOdometer() const;
            encoder_type getEncoderValue() const { return m_last_enc_value-m_odometer_base; }

            class Servo *servo() const { return m_servo; }

        protected:
            void init();
            void cleanup();

            void update();

            friend class Control;
        private:
            std::shared_ptr<::Robot::Context> m_context;
            bool m_initialized;
            const uint m_index;
            class Servo *m_servo;

            bool m_enabled;
            Mode m_mode;

            encoder_type m_last_enc_value;
            clock_type::time_point m_last_update;

            encoder_type m_odometer_base;

            float m_duty;
            float m_duty_set;
            float m_target_rpm;
            float m_rpm;

            Robot::Math::PID m_pid;

            inline uint encoderChannel() const;
            inline uint motorChannel() const;

            friend std::ostream &operator<<(std::ostream &os, const Motor &self)
            {
                return os << "Robot::Motor::Motor[" << self.m_index << "]";
            }
    };

}

#endif 

