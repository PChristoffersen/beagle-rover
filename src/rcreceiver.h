#ifndef _RCRECEIVER_H_
#define _RCRECEIVER_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/signals2.hpp>
#include <robotcontrol-ext.h>


class RCReceiver : public boost::enable_shared_from_this<RCReceiver> {
    public:
        typedef boost::signals2::signal<void(uint8_t)> sig_flags_t;
        typedef boost::signals2::signal<void(uint8_t)> sig_rssi_t;

        RCReceiver(boost::shared_ptr<class RobotContext> context);

        void init();
        void cleanup();

        bool isConnected() const { return m_connected; }
        uint8_t getRSSI() const { return m_rssi; }
        uint8_t getFlags() const { return m_flags; }

        sig_flags_t sigFlags;
        sig_rssi_t sigRSSI;
    private:
        boost::asio::steady_timer m_timer;
        uint32_t m_last_counter;
        volatile shm_fbus_t *m_fbus;

        uint8_t m_rssi;
        uint8_t m_flags;
        bool m_connected;

        void timer();
};

#endif 
