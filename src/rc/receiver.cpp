#include "receiver.h"

#include <algorithm>
#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <robotcontext.h>
#include <telemetry/telemetry.h>

#if ROBOT_HAVE_RC

using namespace std::literals;

namespace Robot::RC {

static constexpr uint16_t TELEMETRY_BATTERY { 0x0300 };


/*
flags = 
bit7 = ch17 = digital channel (0x80)
bit6 = ch18 = digital channel (0x40)
bit5 = Frame lost, equivalent red LED on receiver (0x20)
bit4 = failsafe activated (0x10)
bit3 = n/a
bit2 = n/a
bit1 = n/a
bit0 = n/a
*/


static constexpr auto TIMER_INTERVAL { 10ms };


Receiver::Receiver(const std::shared_ptr<Robot::Context> &context) :
    m_context { context },
    m_initialized { false },
    m_enabled { false },
    m_timer { context->io() },
    m_rssi { 0 },
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    m_fbus { nullptr },
    #endif
    m_last_counter { std::numeric_limits<uint32_t>::max() }
{
}


Receiver::~Receiver() 
{
    cleanup();
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void Receiver::init(const std::shared_ptr<Robot::Telemetry::Telemetry> &telemetry) 
{    
    const guard lock(m_mutex);

    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    m_fbus = rc_ext_fbus_get_shm();
    #endif

    m_telemetry_connection = telemetry->sig_event.connect([&](const auto &e){ telemetryEvent(e); });

    m_initialized = true;
}


void Receiver::cleanup() 
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;
    m_telemetry_connection.disconnect();
    m_timer.cancel();

    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    m_fbus = nullptr;
    #endif
}


void Receiver::setEnabled(bool enabled)
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(info) <<  "Receiver enabled=" << enabled;

    if (enabled!=m_enabled) {
        m_enabled = enabled;

        if (m_enabled) {
            m_timer.expires_after(TIMER_INTERVAL);
            timerSetup();
            m_context->rcPower(true);
        }
        else {
            m_timer.cancel();
            m_context->rcPower(false);
        }
    }
}


void Receiver::timerSetup() {
    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(
        [self_ptr=weak_from_this()] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            if (auto self = self_ptr.lock()) { 
                self->timer(); 
            }
        }
    );
}


#if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
void Receiver::timer() 
{
    const guard lock(m_mutex);

    if (!m_initialized || !m_fbus) {
        return;
    }

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

        auto chsize = m_fbus->n_channels;
        if (chsize>MAX_CHANNELS) {
            chsize = MAX_CHANNELS;
        }
        for (auto i=0; i<chsize; i++) {
            m_channels[i] = m_fbus->channels[i];
        }
        m_channels.setCount(chsize);

        // Signal data
        sigData(m_flags, m_rssi, m_channels);
        if (sig_flags) {
            sigFlags(m_flags);
        }
        if (sig_rssi)
            sigRSSI(m_rssi);

        // Print data
#if 1
        static std::chrono::high_resolution_clock::time_point last_update;
        auto time { std::chrono::high_resolution_clock::now() };
        
        if ((time-last_update) > 200ms) {
            BOOST_LOG_TRIVIAL(info) << boost::format("%+08d f=%+02x r=%+02x ch=%d ") % (uint32_t)counter % (uint32_t)m_flags % (uint32_t)m_rssi % (uint32_t)m_channels.count()
                << " | " << m_channels[0]
                << " | " << m_channels[1]
                << " | " << m_channels[2]
                << " | " << m_channels[3]
                << " |"
                ;

            last_update = time;
        }
#endif
        m_last_counter = counter;
    }

    timerSetup();
}

#else
void Receiver::timer() 
{
    const guard lock(m_mutex);

    if (!m_initialized) {
        return;
    }
}
#endif



void Receiver::telemetryEvent(const Robot::Telemetry::Event &event) 
{
    if (const auto ev = dynamic_cast<const Robot::Telemetry::EventBattery*>(&event)) {
        auto n_cells = ev->cell_voltage.size();
        for (size_t i=0; i<n_cells; i+=2) {
            uint32_t cv1 = ev->cell_voltage[i]*500;
            uint32_t cv2 = 0;
            if (i+1<n_cells) {
                cv2 = ev->cell_voltage[i]*500;
            }
            uint32_t data = ((uint32_t) cv1 & 0x0fff) << 20 | ((uint32_t) cv2 & 0x0fff) << 8 | n_cells << 4 | ev->battery_id;
            sendTelemetry(TELEMETRY_BATTERY + i/2, data);
        }
    }
}

void Receiver::sendTelemetry(uint16_t appId, uint32_t data) 
{
    const guard lock(m_mutex);
    if (!m_enabled || m_flags.frameLost())
        return;
    
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    rc_ext_fbus_send_telemetry(appId, data);
    #endif
}


};

#endif
