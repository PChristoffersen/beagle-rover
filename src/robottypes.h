#ifndef _ROBOTTYPES_H_
#define _ROBOTTYPES_H_

#include <iostream>
#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <boost/format.hpp>

namespace Robot {

    class InputValue {
        public:
            using value_t = std::uint32_t;

            static constexpr std::uint32_t PULSE_MIN { 500u };
            static constexpr std::uint32_t PULSE_MAX { 2500u };
            static constexpr std::int32_t PULSE_CENTER { (PULSE_MAX+PULSE_MIN)/2 };
            static constexpr std::int32_t PULSE_RANGE { (PULSE_MAX-PULSE_MIN) };
            static constexpr std::uint32_t UNSET_VALUE { 0 };

            constexpr InputValue() : m_value { UNSET_VALUE } { }
            constexpr InputValue(value_t v) : m_value { clamp(v) } { }
            constexpr InputValue(InputValue &other) : m_value { other.m_value } { }
            constexpr InputValue(InputValue &&other) : m_value { other.m_value } { }

            InputValue &operator=(const InputValue &other) { m_value = other.m_value; return *this; }
            InputValue &operator=(const std::uint32_t value) { fromMicroSeconds(value); return *this; }


            inline InputValue clamp(value_t min, value_t max) const {
                return InputValue { std::clamp(m_value, min, max) };
            }

            inline static InputValue fromMicroSeconds(std::uint32_t us)
            {
                return InputValue { us };
            }
            inline static InputValue fromPercent(double percent)
            {
                return InputValue { (value_t)((percent * PULSE_RANGE) + PULSE_MIN) };
            }
            inline static InputValue fromAngleRadians(double angle) 
            {
                return InputValue { (value_t)((angle * PULSE_RANGE) / M_PI + PULSE_CENTER) };
            }
            inline static InputValue fromAngle(double angle) 
            {
                return fromAngleRadians(angle * M_PI / 180.0);
            }



            inline value_t asServoPulse() const 
            { 
                return m_value; 
            }
            inline double asPercent() const 
            {
                return (double)((std::int32_t)m_value-PULSE_MIN)/PULSE_RANGE;
            }
            inline double asAngleRadians() const 
            { 
                return M_PI * 2.0 * (double)((int32_t)m_value - PULSE_CENTER) / PULSE_RANGE;
            }
            inline double asAngle() const 
            { 
                return asAngleRadians() * 180.0 / M_PI;
            }


            inline operator bool() const 
            {
                return m_value != UNSET_VALUE;
            }

            operator value_t() const 
            { 
                return asServoPulse(); 
            }


            static const InputValue UNSET;
            static const InputValue CENTER;
            static const InputValue MIN;
            static const InputValue MAX;

        private:
            std::uint32_t m_value;

            inline constexpr static value_t clamp(value_t v) {
                return std::clamp(v, PULSE_MIN, PULSE_MAX);
            }

            friend auto &operator<<(std::ostream &os, const InputValue &v) {
                return os << boost::format("%+4d") % v.m_value;
            }

    };

    constexpr InputValue InputValue::UNSET { InputValue::UNSET_VALUE };
    constexpr InputValue InputValue::CENTER { InputValue::PULSE_CENTER };
    constexpr InputValue InputValue::MIN { InputValue::PULSE_MIN };
    constexpr InputValue InputValue::MAX { InputValue::PULSE_MAX };

};

#endif
