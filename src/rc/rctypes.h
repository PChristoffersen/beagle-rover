#ifndef _RCTYPES_H_
#define _RCTYPES_H_

#include <cstdint>
#include <memory>
#include <algorithm>
#include <array>
#include <boost/signals2.hpp>

#include "../robottypes.h"

namespace Robot::RC {

    class Receiver;

    static constexpr auto MAX_CHANNELS { 24u };

    union Flags {
        std::uint8_t value;
        struct {
            bool ch17:1;
            bool ch18:1;
            bool frame_lost:1;
            bool failsafe:1;
            bool _reserved3:1;
            bool _reserved2:1;
            bool _reserved1:1;
            bool _reserved0:1;
        } bits;
        constexpr Flags() : value{ 0x00 } {}
        constexpr Flags(const std::uint8_t v) : value{v} {}
        bool operator!=(const Flags &other) { return value!=other.value; }
        bool operator!=(const std::uint8_t other) { return value!=other; }
    };


    using _ChannelArray = std::array<Robot::InputValue, MAX_CHANNELS>;

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
                    std::fill(begin()+count, end(), Robot::InputValue::UNSET_VALUE);
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
