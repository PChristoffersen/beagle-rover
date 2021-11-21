#ifndef _RCRECEIVER_H_
#define _RCRECEIVER_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <chrono>
#include <array>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <robotcontrolext.h>

#include "../common/withmutex.h"
#include "../robotcontext.h"
#include "../robottypes.h"

namespace Robot::RC {

    class Receiver : public std::enable_shared_from_this<Receiver>, public WithMutex<std::recursive_mutex> {
        public:
            union Flags {
                std::byte value;
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
                Flags() : value{ 0x00 } {}
                Flags(const std::uint8_t v) : value{v} {}
                Flags(const std::byte v) : value{v} {}
                bool operator!=(const Flags &other) { return value!=other.value; }
                bool operator!=(const std::uint8_t other) { return value!=(std::byte)other; }
            };

            static constexpr auto MAX_CHANNELS { 24u };

            class ChannelList : public std::array<Robot::InputValue, MAX_CHANNELS> {
                public:
                    ChannelList();
                    std::size_t count() const { return m_count; }
                protected:
                    friend class Receiver;
                    void setCount(std::size_t count);
                private:
                    std::size_t m_count;
            };

            //using Flags = std::byte;
            using SignalFlags = boost::signals2::signal<void(Flags)>;
            using SignalRSSI = boost::signals2::signal<void(std::uint8_t)>;
            using SignalData = boost::signals2::signal<void(Flags flags, std::uint8_t rssi, const ChannelList &channels)>;

            SignalFlags sigFlags;
            SignalRSSI sigRSSI;
            SignalData sigData;

            explicit Receiver(std::shared_ptr<Robot::Context> context);
            Receiver(const Receiver&) = delete; // No copy constructor
            Receiver(Receiver&&) = delete; // No move constructor
            virtual ~Receiver();

            void init();
            void cleanup();

            bool isConnected() const { return m_connected; }
            std::uint8_t getRSSI() const { return m_rssi; }
            Flags getFlags() const { return m_flags; }

            const ChannelList &channels() const { return m_channels; }

        private:
            bool m_initialized;
            boost::asio::steady_timer m_timer;
            std::uint32_t m_last_counter;
            volatile shm_fbus_t *m_fbus;

            ChannelList m_channels;
            std::uint8_t m_rssi;
            Flags m_flags;
            bool m_connected;

            void timer_setup();
            void timer(boost::system::error_code error);
    };
 
};


#endif 
