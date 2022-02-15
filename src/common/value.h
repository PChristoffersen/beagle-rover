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

            static const Value CENTER;
            static const Value MIN;
            static const Value MAX;
            
            constexpr Value() : m_value { PULSE_CENTER } { }
            constexpr Value(const Value &other) : m_value { other.m_value } { }
            constexpr Value(const Value &&other) : m_value { other.m_value } { }
            constexpr Value(value_t v) : m_value { clamp(v) } { }
            constexpr Value(std::int32_t v) : Value { (value_t)v } { }

            Value &operator=(const Value &other) { m_value = other.m_value; return *this; }
            Value &operator=(const std::uint32_t value) 
            { 
                m_value = clamp(value); 
                return *this; 
            }

            Value constexpr operator-() const { 
                return (PULSE_MAX-m_value+PULSE_MIN);
            }

            inline constexpr bool operator==(const Value& other) const 
            { 
                return m_value == other.m_value;
            }
            inline constexpr bool operator!=(const Value& other) const 
            {
                return m_value != other.m_value;
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
                return Value { (value_t)((std::clamp(percent, 0.0f, 1.0f) * PULSE_RANGE) + PULSE_MIN) };
            }
            inline static constexpr Value fromFloat(float value)
            {
                return Value { (value_t)(((1.0f+std::clamp(value, -1.0f, 1.0f)) * 0.5f * PULSE_RANGE) + PULSE_MIN) };
            }
            inline static constexpr Value fromAngle(float angle) 
            {
                return Value { (value_t)((std::clamp(angle, -(float)M_PI_2, (float)M_PI_2) * PULSE_RANGE) / (float)M_PI + PULSE_CENTER) };
            }
            inline static constexpr Value fromAngleDegrees(float angle) 
            {
                return fromAngle(angle * (float)M_PI / 180.0f);
            }



            inline constexpr value_t asServoPulse() const 
            { 
                return m_value; 
            }
            inline constexpr float asPercent() const 
            {
                return (float)(m_value-PULSE_MIN)/PULSE_RANGE;
            }
            inline constexpr float asFloat() const 
            {
                return (float)(2*((std::int32_t)m_value-PULSE_CENTER))/PULSE_RANGE;
            }
            inline constexpr float asAngle() const 
            { 
                return (float)M_PI * (float)((int32_t)m_value - PULSE_CENTER) / PULSE_RANGE;
            }
            inline constexpr float asAngleDegrees() const 
            { 
                return asAngle() * 180.0f / (float)M_PI;
            }
            inline constexpr uint8_t asButton(uint8_t divisions) const 
            {
                auto range = PULSE_RANGE/(divisions-1);
                return (m_value-PULSE_MIN+range/2) / range;
            }
            inline constexpr bool asToggle() const
            {
                return asButton(2);
            }

            constexpr operator value_t() const 
            { 
                return asServoPulse(); 
            }
            constexpr operator float() const {
                return asFloat();
            }

        private:
            std::uint32_t m_value;

            inline constexpr static value_t clamp(value_t v) {
                return std::clamp(v, PULSE_MIN, PULSE_MAX);
            }

            friend auto &operator<<(std::ostream &os, const Value &v) {
                return os << boost::format("%+4d") % v.m_value;
            }

    };

    constexpr Value Value::CENTER { Value::PULSE_CENTER };
    constexpr Value Value::MIN { Value::PULSE_MIN };
    constexpr Value Value::MAX { Value::PULSE_MAX };
};

#endif