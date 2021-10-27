#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "rcreceiver.h"
#include "../robotcontext.h"

using namespace std;


static constexpr auto TIMER_INTERVAL = chrono::milliseconds(100);

/*
flags = bit7 = ch17 = digital channel (0x80)
bit6 = ch18 = digital channel (0x40)
bit5 = Frame lost, equivalent red LED on receiver (0x20)
bit4 = failsafe activated (0x10)
bit3 = n/a
bit2 = n/a
bit1 = n/a
bit0 = n/a

*/
static constexpr uint8_t FBUS_FLAG_SIGNAL_LOSS { 1 << 2 };
static constexpr uint8_t FBUS_FLAG_FAILSAFE_ACTIVE { 1 << 3 };


RCReceiver::RCReceiver(shared_ptr<RobotContext> context) :
    m_initialized { true },
    m_timer { context->io() },
    m_connected { false },
    m_rssi { 0 }
{

}


RCReceiver::~RCReceiver() 
{
    cleanup();
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void RCReceiver::init() 
{    
    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
        m_fbus = rc_ext_fbus_get_shm();
        break;
    default:
        break;
    }

    m_timer.expires_after(TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&RCReceiver::timer, this, _1));

    m_initialized = true;
}


void RCReceiver::cleanup() 
{
    if (!m_initialized) 
        return;
    m_initialized = false;
    m_timer.cancel();
    m_fbus = nullptr;
}



void RCReceiver::timer(boost::system::error_code error) 
{
    if (error!=boost::system::errc::success || !m_initialized) {
        return;
    }

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
                cout << format("%+4d |") % ch;
            }
            cout << "  " << (m_fbus->channels[0]-500) << "   "  << ((m_fbus->channels[0]-500)/2000.0) << "          ";
            cout << "\r" << flush;
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
    m_timer.async_wait(boost::bind(&RCReceiver::timer, this, _1));
}
