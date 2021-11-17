#ifndef _RCRECEIVER_H_
#define _RCRECEIVER_H_

#include <cstdint>
#include <memory>
#include <mutex>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <robotcontrolext.h>

#include "../common/withmutex.h"

class RCReceiver : public std::enable_shared_from_this<RCReceiver>, public WithMutex<std::recursive_mutex> {
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

        using ChannelList = std::vector<std::uint32_t> ;
        //using Flags = std::byte;
        using SignalFlags = boost::signals2::signal<void(Flags)>;
        using SignalRSSI = boost::signals2::signal<void(std::uint8_t)>;
        using SignalData = boost::signals2::signal<void(Flags flags, std::uint8_t rssi, const ChannelList &channels)>;

        SignalFlags sigFlags;
        SignalRSSI sigRSSI;
        SignalData sigData;

        explicit RCReceiver(std::shared_ptr<class RobotContext> context);
        RCReceiver(const RCReceiver&) = delete; // No copy constructor
        RCReceiver(RCReceiver&&) = delete; // No move constructor
        virtual ~RCReceiver();

        void init();
        void cleanup();

        bool isConnected() const { return m_connected; }
        std::uint8_t getRSSI() const { return m_rssi; }
        Flags getFlags() const { return m_flags; }

        ChannelList getChannels() { 
            const std::lock_guard<std::recursive_mutex> lock(m_mutex);
            return m_channels; 
        }
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

#endif 
