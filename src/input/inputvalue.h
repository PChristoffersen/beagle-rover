#ifndef _ROBOTINPUT_H_
#define _ROBOTINPUT_H_

#include <iostream>
#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <boost/format.hpp>

namespace Robot::Input {

    class Value {
        public:
            using value_t = std::uint32_t;

            static constexpr std::uint32_t PULSE_MIN { 500u };
            static constexpr std::uint32_t PULSE_MAX { 2500u };
            static constexpr std::int32_t PULSE_CENTER { (PULSE_MAX+PULSE_MIN)/2 };
            static constexpr std::int32_t PULSE_RANGE { (PULSE_MAX-PULSE_MIN) };
            static constexpr std::uint32_t UNSET_VALUE { 0 };

            static const Value UNSET;
            static const Value CENTER;
            static const Value MIN;
            static const Value MAX;
            
            constexpr Value() : m_value { UNSET_VALUE } { }
            constexpr Value(const Value &other) : m_value { other.m_value } { }
            constexpr Value(const Value &&other) : m_value { other.m_value } { }
            constexpr Value(value_t v) : m_value { clamp(v) } { }

            Value &operator=(const Value &other) { m_value = other.m_value; return *this; }
            Value &operator=(const std::uint32_t value) 
            { 
                if (value==UNSET_VALUE)
                    m_value = UNSET_VALUE;
                else
                    m_value = clamp(value); 
                return *this; 
            }

            Value constexpr operator-() const { 
                if (m_value != UNSET_VALUE)
                    return (PULSE_MAX-m_value+PULSE_MIN);
                else
                    return UNSET;
            }

            inline constexpr Value clamp(value_t min, value_t max) const {
                return Value { std::clamp(m_value, min, max) };
            }

            inline static constexpr Value fromMicroSeconds(std::uint32_t us)
            {
                return Value { us };
            }
            inline static constexpr Value fromPercent(double percent)
            {
                return Value { (value_t)((percent * PULSE_RANGE) + PULSE_MIN) };
            }
            inline static constexpr Value fromAngleRadians(double angle) 
            {
                return Value { (value_t)((angle * PULSE_RANGE) / M_PI + PULSE_CENTER) };
            }
            inline static constexpr Value fromAngle(double angle) 
            {
                return fromAngleRadians(angle * M_PI / 180.0);
            }



            inline constexpr value_t asServoPulse() const 
            { 
                return m_value; 
            }
            inline constexpr double asPercent() const 
            {
                return (double)((std::int32_t)m_value-PULSE_CENTER)/PULSE_RANGE;
            }
            inline constexpr double asAngleRadians() const 
            { 
                return M_PI * (double)((int32_t)m_value - PULSE_CENTER) / PULSE_RANGE;
            }
            inline constexpr double asAngle() const 
            { 
                return asAngleRadians() * 180.0 / M_PI;
            }


            inline constexpr operator bool() const 
            {
                return m_value != UNSET_VALUE;
            }

            constexpr operator value_t() const 
            { 
                return asServoPulse(); 
            }

        private:
            std::uint32_t m_value;

            inline constexpr static value_t clamp(value_t v) {
                return std::clamp(v, PULSE_MIN, PULSE_MAX);
            }

            friend auto &operator<<(std::ostream &os, const Value &v) {
                if (v)
                    return os << boost::format("%+4d") % v.m_value;
                else
                    return os << "<unset>";
            }

    };

    constexpr Value Value::UNSET { };
    constexpr Value Value::CENTER { Value::PULSE_CENTER };
    constexpr Value Value::MIN { Value::PULSE_MIN };
    constexpr Value Value::MAX { Value::PULSE_MAX };
};

#endif