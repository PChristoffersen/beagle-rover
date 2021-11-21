#ifndef _MOTORGIMBAL_H_
#define _MOTORGIMBAL_H_

#include <cstdint>
#include <memory>
#include <chrono>
#include <mutex>
#include "../robottypes.h"

namespace Robot::Motor {

    class Gimbal {
        public:
            Gimbal(uint index, std::recursive_mutex &mutex);
            Gimbal(const Gimbal&) = delete; // No copy constructor
            Gimbal(Gimbal&&) = delete; // No move constructor
            virtual ~Gimbal();

            uint getIndex() const { return m_index; }

            void setEnabled(bool enable);
            bool getEnabled() const { return m_enabled; }

            bool getPassthrough() const { return m_passthrough; }

            void setValue(const Robot::InputValue value);
            Robot::InputValue getValue() const { return m_value; }

            void setPulseUS(std::uint32_t us);
            std::uint32_t getPulseUS() const { return m_value.asServoPulse(); }

            void setAngle(double angle);
            double getAngle() const;

            void setAngleRadians(double angle);
            double getAngleRadians() const;

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
            bool m_initialized;
            uint m_index;
            std::recursive_mutex &m_mutex;
            bool m_enabled;
            bool m_passthrough;
            InputValue m_value;
            std::chrono::high_resolution_clock::time_point m_last_pulse;

            std::uint32_t m_limit_min;
            std::uint32_t m_limit_max;
            std::int32_t  m_trim;

            inline uint servoChannel() const;
    };

};

#endif
