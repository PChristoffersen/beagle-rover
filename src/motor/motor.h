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
#include <common/withnotify.h>
#include <telemetry/types.h>
#include <telemetry/events.h>
#include <telemetry/telemetrysource.h>
#include "types.h"

namespace Robot::Motor {

    class Motor : private Robot::Telemetry::Source, public WithNotifyDefault {
        public:
            static constexpr NotifyType NOTIFY_TELEMETRY { "telemetry" };

            using clock = std::chrono::high_resolution_clock;
            using mutex_type = std::recursive_mutex;
            using guard = std::lock_guard<mutex_type>;

            enum class Mode {
                DUTY,
                RPM,
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

            void setValue(const Value value);
            void setDuty(float duty);
            float getDuty() const { return m_duty; }
            void setTargetRPM(float rpm);
            float getTargetRPM() const { return m_target_rpm; }

            Mode getMode() const { return m_mode; }
            float getRPM() const { return m_rpm; }

            void resetOdometer();
            double getOdometer() const;
            int32_t getEncoderValue() const { return m_last_enc_value-m_odometer_base; }

            class Servo *servo() const { return m_servo.get(); }

        protected:
            void init(const std::shared_ptr<Robot::Telemetry::Telemetry> &telemetry);
            void cleanup();

            void update();

            friend class Control;
        private:
            std::shared_ptr<::Robot::Context> m_context;
            bool m_initialized;
            const uint m_index;
            mutex_type &m_mutex;
            std::unique_ptr<class Servo> m_servo;

            bool m_enabled;
            Mode m_mode;

            std::int32_t m_last_enc_value;
            clock::time_point m_last_update;

            std::int32_t m_odometer_base;

            float m_duty;
            float m_duty_set;
            float m_target_rpm;
            float m_rpm;

            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            rc_filter_t m_rc_pid;
            #endif
            Robot::Math::PID m_pid;

            Robot::Telemetry::EventMotor m_event;

            inline uint encoderChannel() const;
            inline uint motorChannel() const;

            friend std::ostream &operator<<(std::ostream &os, const Motor &self)
            {
                return os << "Motor::Motor[" << self.m_index << "]";
            }
    };

};

#endif 

