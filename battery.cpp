#include <iostream>
#include <boost/bind.hpp>

#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "battery.h"


using namespace boost;
using namespace boost::asio;

#define TIMER_INTERVAL 1000

using namespace boost;
using namespace boost::asio;


Battery::Battery(io_context &io):
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
    float cells[2] = { pack_voltage/2.0f, pack_voltage/2.0f };
    rc_ext_fbus_telemetry_send_battery(0x00, cells, 2);

    m_timer.expires_at(m_timer.expiry() + chrono::milliseconds(TIMER_INTERVAL));
    m_timer.async_wait(bind(&Battery::timer, this));
}
