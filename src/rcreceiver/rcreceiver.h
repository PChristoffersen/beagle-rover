#ifndef _RCRECEIVER_H_
#define _RCRECEIVER_H_

#include <memory>
#include <mutex>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <robotcontrolext.h>


class RCReceiver : public std::enable_shared_from_this<RCReceiver> {
    public:
        typedef std::vector<uint32_t> ChannelList;
        typedef boost::signals2::signal<void(uint8_t)> SignalFlags;
        typedef boost::signals2::signal<void(uint8_t)> SignalRSSI;
        typedef boost::signals2::signal<void(uint8_t flags, uint8_t rssi, const ChannelList &channels)> SignalData;

        RCReceiver(std::shared_ptr<class RobotContext> context);
        virtual ~RCReceiver();

        void init();
        void cleanup();

        bool isConnected() const { return m_connected; }
        uint8_t getRSSI() const { return m_rssi; }
        uint8_t getFlags() const { return m_flags; }

        const ChannelList &getChannels() const { return m_channels; }

        SignalFlags sigFlags;
        SignalRSSI sigRSSI;
        SignalData sigData;
    private:
        static constexpr auto TIMER_INTERVAL { std::chrono::milliseconds(100) };

        bool m_initialized;
        std::mutex m_mutex;
        boost::asio::steady_timer m_timer;
        uint32_t m_last_counter;
        volatile shm_fbus_t *m_fbus;

        ChannelList m_channels;
        uint8_t m_rssi;
        uint8_t m_flags;
        bool m_connected;

        void timer_setup();
        void timer(boost::system::error_code error);
};

#endif 
