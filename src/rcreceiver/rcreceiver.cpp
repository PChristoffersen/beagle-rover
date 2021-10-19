#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "rcreceiver.h"
#include "../robotcontext.h"

using namespace std::chrono;
using namespace boost;
using namespace boost::asio;
using namespace boost::signals2;


#define TIMER_INTERVAL milliseconds(20)


RCReceiver::RCReceiver(shared_ptr<RobotContext> context) :
    m_timer(*(context->io())),
    m_connected(false),
    m_rssi(0)
{

}


void RCReceiver::init() {
    m_fbus = rc_ext_fbus_get_shm();
    rc_servo_power_rail_en(1);

    m_timer.expires_after(TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&RCReceiver::timer, this));
}


void RCReceiver::cleanup() {
    m_timer.cancel();
    m_fbus = NULL;
}



void RCReceiver::timer() {
    if (!m_fbus) 
        return;
        
    uint16_t counter = m_fbus->counter;

    if (counter != m_last_counter) {
        bool sig_flags = false;
        bool sig_rssi = false;

        // TODO check connected
        if (m_fbus->flags != m_flags) {
            m_flags = m_fbus->flags;
            sig_flags = true;
        }

        if (m_fbus->rssi != m_rssi) {
            m_rssi = m_fbus->rssi;
            sig_rssi = true;
        }


#if 0
        if ((counter%20)==0) {
            BOOST_LOG_TRIVIAL(info) << format("%+04x f=%+02x  r=%+02x  ch=%d   ") % (uint32_t)m_fbus->counter % (uint32_t)m_fbus->flags % (uint32_t)m_fbus->rssi % m_fbus->n_channels
                << format("%+4d |") % m_fbus->channels[0] << " |"
                << format("%+4d |") % m_fbus->channels[1] << " |"
                << format("%+4d |") % m_fbus->channels[2] << " |"
                << format("%+4d |") % m_fbus->channels[3] << " |" 
                ;

            #if 0
            for (int i=0; i<8; i++) {
                uint32_t ch = m_fbus->channels[i];
                std::cout << format("%+4d |") % ch;
            }
            std::cout << "  " << (m_fbus->channels[0]-500) << "   "  << ((m_fbus->channels[0]-500)/2000.0) << "          ";
            std::cout << "\r" << std::flush;
            #endif
        }
#endif


        m_last_counter = counter;

        //if (sig_flags)
        //    sigFlags(m_flags);
        //if (sig_rssi)
        //    sigRSSI(m_rssi);
    }

    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&RCReceiver::timer, this));
}
