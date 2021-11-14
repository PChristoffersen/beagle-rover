#include "rcreceiver.h"

#include <algorithm>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "../robotcontext.h"

using namespace std;



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

static constexpr auto TIMER_INTERVAL { 10ms };

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
    const lock_guard<mutex> lock(m_mutex);

    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
        m_fbus = rc_ext_fbus_get_shm();
        break;
    default:
        break;
    }

    m_timer.expires_after(TIMER_INTERVAL);
    timer_setup();

    m_initialized = true;
}


void RCReceiver::cleanup() 
{
    const lock_guard<mutex> lock(m_mutex);

    if (!m_initialized) 
        return;
    m_initialized = false;
    m_timer.cancel();
    m_fbus = nullptr;
}


void RCReceiver::timer_setup() {
    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(
        [self_ptr=weak_from_this()] (auto &error) {
            if (auto self = self_ptr.lock()) { 
                self->timer(error); 
            }
        }
    );
}


void RCReceiver::timer(boost::system::error_code error) 
{
    const lock_guard<mutex> lock(m_mutex);

    if (error!=boost::system::errc::success || !m_initialized) {
        return;
    }

    if (!m_fbus) 
        return;
        
    uint32_t counter = m_fbus->counter;
    
    if (counter != m_last_counter) {
        bool sig_flags = false;
        bool sig_rssi = false;

        // Collect data
        if (m_flags != m_fbus->flags) {
            m_flags = m_fbus->flags;
            sig_flags = true;
        }

        if (m_fbus->rssi != m_rssi) {
            m_rssi = m_fbus->rssi;
            sig_rssi = true;
        }

        auto chsize = m_channels.size();
        if (chsize!=m_fbus->n_channels) {
            chsize = m_fbus->n_channels;
            m_channels.resize(chsize);
        }
        for (auto i=0; i<chsize; i++) {
            m_channels[i] = m_fbus->channels[i];
        }

        // SIgnal data
        sigData(m_flags, m_rssi, m_channels);
#if 1
        static chrono::high_resolution_clock::time_point last_update;
        auto time = chrono::high_resolution_clock::now();
        if ((time-last_update) > 100ms) {


            BOOST_LOG_TRIVIAL(info) << boost::format("%+04x f=%+02x  r=%+02x  ch=%d   ") % (uint32_t)counter % (uint32_t)m_flags.value % (uint32_t)m_rssi % (uint32_t)m_channels.size()
                << boost::format("%+4d |") % m_channels[0]
                << boost::format("%+4d |") % m_channels[1]
                << boost::format("%+4d |") % m_channels[2]
                << boost::format("%+4d |") % m_channels[3]
                ;

            #if 0
            for (int i=0; i<8; i++) {
                uint32_t ch = m_fbus->channels[i];
                cout << format("%+4d |") % ch;
            }
            cout << "  " << (m_fbus->channels[0]-500) << "   "  << ((m_fbus->channels[0]-500)/2000.0) << "          ";
            cout << "\r" << flush;
            #endif

            last_update = time;
        }
#endif
        m_last_counter = counter;

        //if (sig_flags)
        //    sigFlags(m_flags);
        //if (sig_rssi)
        //    sigRSSI(m_rssi);
    }

    timer_setup();
}
