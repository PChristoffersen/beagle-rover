#ifndef _FBUS_H_
#define _FBUS_H_

#include "component.h"

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/signals2.hpp>
#include <robotcontrol-ext.h>


class FBus : public Component, public boost::enable_shared_from_this<FBus> {
    public:
        typedef boost::signals2::signal<void(bool)> sig_connection_t;
        typedef boost::signals2::signal<void(uint8_t)> sig_rssi_t;

        FBus(boost::shared_ptr<boost::asio::io_context> io);

        void init() override;
        void cleanup() override;

        bool connected() { return m_connected; }
        uint8_t rssi() { return m_rssi; }

        sig_connection_t signal_connection;
        sig_rssi_t signal_rssi;
    private:

        boost::asio::steady_timer m_timer;
        uint16_t m_last_counter;
        volatile shm_fbus_t *m_fbus;

        uint8_t m_rssi;
        bool m_connected;

        void timer();
};

#endif 
