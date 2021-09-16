#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "fbus.h"

using namespace std;
using namespace std::chrono;
using namespace boost;
using namespace boost::asio;
using namespace boost::signals2;


#define TIMER_INTERVAL milliseconds(200)


FBus::FBus(io_context &io) :
    m_timer(io),
    m_connected(false),
    m_rssi(0)
{

}


void FBus::init() {
    m_timer.expires_after(TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&FBus::timer, this));
    Component::init();
}


void FBus::cleanup() {
    m_timer.cancel();
    Component::cleanup();
}



void FBus::timer() {
    uint16_t counter = m_fbus->counter;

    if (counter != m_last_counter) {

        // TODO check connected
        //if (m_fbus->flags)

        if (m_fbus->rssi != m_rssi) {
            m_rssi = m_fbus->rssi;
            signal_rssi(m_rssi);
        }



        if ((counter%20)==0) {
            std::cout << format("%+04x f=%+02x  r=%+02x   ") % (uint32_t)m_fbus->counter % (uint32_t)m_fbus->flags % (uint32_t)m_fbus->rssi;
            for (int i=0; i<8; i++) {
                uint32_t ch = m_fbus->channels[i];
                std::cout << format("%+4d |") % ch;
            }
            std::cout << "  " << (m_fbus->channels[0]-500) << "   "  << ((m_fbus->channels[0]-500)/2000.0) << "          ";
            std::cout << "\r" << std::flush;
        }

        m_last_counter = counter;
    }

    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&FBus::timer, this));
}
