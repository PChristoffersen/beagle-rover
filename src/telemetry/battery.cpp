#include <iostream>
#include <boost/bind.hpp>

#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "battery.h"
#include "telemetry.h"
#include "telemetrytypes.h"
#include "../robotcontext.h"


using namespace std::chrono;
using namespace boost;
using namespace boost::asio;

#define TIMER_INTERVAL milliseconds(1000)


Battery::Battery(shared_ptr<RobotContext> context):
    m_timer(*(context->io()))
{

}

Battery::~Battery() {
    cleanup();
}


void Battery::init() {
    m_timer.expires_after(TIMER_INTERVAL);
    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
        m_timer.async_wait(bind(&Battery::timer, this));
        break;
    default:
        m_timer.async_wait(bind(&Battery::timerFake, this));
        break;
    }
}


void Battery::cleanup() {
    m_timer.cancel();
}

void Battery::timer() {
    float pack_voltage = rc_adc_batt();
    if (pack_voltage<0.0) {
        return;
    }
    
    TelemetryEventBattery event;
    event.battery_id = 0x00;
    event.cell_voltage.push_back(pack_voltage/2.0f);
    event.cell_voltage.push_back(pack_voltage/2.0f);

    sig_event(event);

    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(bind(&Battery::timer, this));
}

void Battery::timerFake() {
    TelemetryEventBattery event;
    event.battery_id = 0x00;
    event.cell_voltage.push_back(4.2f);
    event.cell_voltage.push_back(4.1f);

    sig_event(event);

    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(bind(&Battery::timerFake, this));
}