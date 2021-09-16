#include <iostream>
#include <boost/bind.hpp>

#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "battery.h"
#include "telemetry.h"
#include "telemetrytypes.h"


using namespace boost;
using namespace boost::asio;

#define TIMER_INTERVAL 1000

using namespace boost;
using namespace boost::asio;


Battery::Battery(io_context &io, shared_ptr<Telemetry> telemetry):
    m_telemetry(telemetry),
    m_timer(io)
{

}

Battery::~Battery() {
    if (m_initialized) {
        cleanup();
    }
}


void Battery::init() {
    m_timer.expires_after(chrono::milliseconds(TIMER_INTERVAL));
    m_timer.async_wait(bind(&Battery::timer, this));
    Component::init();
}


void Battery::cleanup() {
    m_timer.cancel();
    Component::cleanup();
}

void Battery::timer() {
    float pack_voltage = rc_adc_batt();
    if (pack_voltage<0.0) {
        return;
    }
    
    uint8_t battery_id = 0x00;
    int n_cells = 2;
    float cells[n_cells] = { pack_voltage/2.0f, pack_voltage/2.0f };

    for (int i=0; i<n_cells; i+=2) {
        uint32_t cv1 = cells[i]*500;
        uint32_t cv2 = 0;
        
        if (i+1<n_cells) {
            cv2 = cells[i]*500;
        }
        
        uint32_t data = ((uint32_t) cv1 & 0x0fff) << 20 | ((uint32_t) cv2 & 0x0fff) << 8 | n_cells << 4 | battery_id;

        if (auto tel = m_telemetry.lock()) {
            tel->send(TELEMETRY_BATTERY + i/2, data);
        }
    }

    m_timer.expires_at(m_timer.expiry() + chrono::milliseconds(TIMER_INTERVAL));
    m_timer.async_wait(bind(&Battery::timer, this));
}
