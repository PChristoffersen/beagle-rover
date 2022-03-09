#ifndef _ROBOT_SERVO_H_
#define _ROBOT_SERVO_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <iostream>
#include <robottypes.h>
#include <common/withnotify.h>
#include <telemetry/types.h>
#include <telemetry/events.h>
#include <telemetry/telemetrysource.h>
#include "types.h"

namespace Robot::Motor {

    class Servo : private Robot::Telemetry::Source, public WithNotifyInt {
        public:
            using mutex_type = std::recursive_mutex;
            using guard = std::lock_guard<mutex_type>;

            Servo(uint index, mutex_type &mutex, const std::shared_ptr<::Robot::Context> &context);
            Servo(const Servo&) = delete; // No copy constructor
            Servo(Servo&&) = delete; // No move constructor
            virtual ~Servo();

            uint getIndex() const { return m_index; }

            void setEnabled(bool enable);
            bool getEnabled() const { return m_enabled; }

            void setValue(const Value value);
            Value getValue() const { return m_value; }

            void setLimits(const Value lmin, const Value lmax);
            void setLimitMin(const Value limit);
            Value getLimitMin() const { return m_limit_min; }
            void setLimitMax(const Value limit);
            Value getLimitMax() const { return m_limit_max; }

        protected:
            void init(const std::shared_ptr<Robot::Telemetry::Telemetry> &telemetry);
            void cleanup();

            void update();

            friend class Motor;
            friend class Control;
        private:
            std::shared_ptr<::Robot::Context> m_context;
            bool m_initialized;
            const uint m_index;
            mutex_type &m_mutex;
            bool m_enabled;
            Value m_value;

            Value m_limit_min;
            Value m_limit_max;
            std::int32_t  m_trim;

            Robot::Telemetry::EventServo m_event;

            inline uint servoChannel() const;

            friend std::ostream &operator<<(std::ostream &os, const Servo &self)
            {
                return os << "Robot::Motor::Servo[" << self.m_index << "]";
            }

    };

}

#endif
