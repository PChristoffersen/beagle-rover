#ifndef _ROBOT_VALUE_H_
#define _ROBOT_VALUE_H_

#include <iostream>
#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <boost/format.hpp>

#include <robotconfig.h>

namespace Robot {

    class Value {
        public:
            using value_t = std::uint32_t;

            static constexpr std::uint32_t PULSE_MIN { ::Robot::Config::SERVO_LIMIT_MIN };
            static constexpr std::uint32_t PULSE_MAX { ::Robot::Config::SERVO_LIMIT_MAX };
            static constexpr std::int32_t PULSE_CENTER { ::Robot::Config::SERVO_CENTER };
            static constexpr std::int32_t PULSE_RANGE { ::Robot::Config::SERVO_RANGE };
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
            inline static constexpr Value fromPercent(float percent)
            {
                return Value { (value_t)((percent * PULSE_RANGE) + PULSE_MIN) };
            }
            inline static constexpr Value fromAngleRadians(float angle) 
            {
                return Value { (value_t)((angle * PULSE_RANGE) / (float)M_PI + PULSE_CENTER) };
            }
            inline static constexpr Value fromAngle(float angle) 
            {
                return fromAngleRadians(angle * (float)M_PI / 180.0f);
            }



            inline constexpr value_t asServoPulse() const 
            { 
                return m_value; 
            }
            inline constexpr float asPercent() const 
            {
                return (float)(2*((std::int32_t)m_value-PULSE_CENTER))/PULSE_RANGE;
            }
            inline constexpr float asAngleRadians() const 
            { 
                return (float)M_PI * (float)((int32_t)m_value - PULSE_CENTER) / PULSE_RANGE;
            }
            inline constexpr float asAngle() const 
            { 
                return asAngleRadians() * 180.0f / (float)M_PI;
            }
            inline constexpr uint asButton(uint divisions) const 
            {
                if (m_value==UNSET_VALUE)
                    return 0;
                auto range = PULSE_RANGE/(divisions-1);
                return (m_value-PULSE_MIN+range/2) / range;
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