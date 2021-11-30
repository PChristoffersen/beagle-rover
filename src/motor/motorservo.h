#ifndef _MOTORSERVO_H_
#define _MOTORSERVO_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <iostream>
#include "../robottypes.h"
#include "motortypes.h"

namespace Robot::Motor {

    class Servo {
        public:
            Servo(uint index, std::recursive_mutex &mutex, const std::shared_ptr<::Robot::Context> &context);
            Servo(const Servo&) = delete; // No copy constructor
            Servo(Servo&&) = delete; // No move constructor
            virtual ~Servo();

            uint getIndex() const { return m_index; }

            void setEnabled(bool enable);
            bool getEnabled() const { return m_enabled; }

            bool getPassthrough() const { return m_passthrough; }

            void setValue(const Value value);
            Value getValue() const { return m_value; }

            void setLimits(std::uint32_t lmin, std::uint32_t lmax);
            void setLimitMin(std::uint32_t limit);
            std::uint32_t getLimitMin() const { return m_limit_min; };
            void setLimitMax(std::uint32_t limit);
            std::uint32_t getLimitMax() const { return m_limit_max; }


        protected:
            void init();
            void cleanup();

            void setPassthrough(bool passthrough);

            void update();

            friend class Motor;
            friend class Control;
        private:
            std::shared_ptr<::Robot::Context> m_context;
            bool m_initialized;
            uint m_index;
            std::recursive_mutex &m_mutex;
            bool m_enabled;
            bool m_passthrough;
            Value m_value;

            std::uint32_t m_limit_min;
            std::uint32_t m_limit_max;
            std::int32_t  m_trim;

            inline uint servoChannel() const;

            friend std::ostream &operator<<(std::ostream &os, const Servo &self)
            {
                return os << "Motor::Servo[" << self.m_index << "]";
            }

    };

};

#endif
