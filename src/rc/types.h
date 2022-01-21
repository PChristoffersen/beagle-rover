#ifndef _ROBOT_RC_TYPES_H_
#define _ROBOT_RC_TYPES_H_

#include <cstdint>
#include <memory>
#include <algorithm>
#include <array>
#include <iostream>
#include <boost/signals2.hpp>

#include <robottypes.h>
#include <common/value.h>

namespace Robot::RC {

    class Receiver;

    static constexpr auto MAX_CHANNELS { 24u };

    class Flags {
        public:
            using value_type = std::uint8_t;

            static constexpr value_type CH17_MASK       { 1<<0 };
            static constexpr value_type CH18_MASK       { 1<<1 };
            static constexpr value_type FRAME_LOST_MASK { 1<<2 };
            static constexpr value_type FAILSAFE_MASK   { 1<<3 };

            static constexpr value_type INITIAL_VALUE { FRAME_LOST_MASK | 0x80 };


            constexpr Flags() : m_value { INITIAL_VALUE } {}
            constexpr Flags(value_type value) : m_value { value } {}
            constexpr Flags(const Flags &other) : m_value { other.m_value } {}
            constexpr Flags(const Flags &&other) : m_value { other.m_value } {}

            Flags &operator=(const Flags &other) { m_value = other.m_value; return *this; }
            Flags &operator=(const value_type value) { m_value = value; return *this; }

            bool operator==(const Flags &other) { return m_value==other.m_value; }
            bool operator==(const value_type value) { return m_value==value; }
            bool operator!=(const Flags &other) { return m_value!=other.m_value; }
            bool operator!=(const value_type value) { return m_value!=value; }

            operator value_type() const { return m_value; }
            operator std::uint32_t() const { return m_value; }

            inline bool ch17() const { return m_value & CH17_MASK; }
            inline bool ch18() const { return m_value & CH18_MASK; }
            inline bool frameLost() const { return m_value & FRAME_LOST_MASK; }
            inline bool failsafe() const { return m_value & FAILSAFE_MASK; }

        private:
            value_type m_value;

            friend std::ostream &operator<<(std::ostream &os, const Flags &flags)
            {
                return os << "{" 
                    //<< " ch17=" << flags.ch17()
                    //<< " ch18=" << flags.ch18()
                    << " frameLost=" << flags.frameLost()
                    << " failsafe=" << flags.failsafe()
                    << " }";
            }
    };


    using Value = ::Robot::Value;
    using _ChannelArray = std::array<Value, MAX_CHANNELS>;

    class ChannelList : public _ChannelArray {
        public:
            ChannelList() : 
                m_count { 0 }
            {
            }
            std::size_t count() const { return m_count; }
        protected:
            friend class Receiver;
            void setCount(std::size_t count) 
            {
                if (m_count!=count) {
                    std::fill(begin()+count, end(), Value::UNSET_VALUE);
                    m_count = count;
                }
            }
        private:
            std::size_t m_count;
    };

    using RSSI = std::uint8_t;

    using SignalFlags = boost::signals2::signal<void(Flags)>;
    using SignalRSSI = boost::signals2::signal<void(RSSI)>;
    using SignalData = boost::signals2::signal<void(Flags flags, RSSI rssi, const ChannelList &channels)>;

};


#endif
