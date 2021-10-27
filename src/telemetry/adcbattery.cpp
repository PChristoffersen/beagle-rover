#include <iostream>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>

#include "adcbattery.h"
#include "telemetry.h"
#include "telemetrytypes.h"
#include "../robotcontext.h"

using namespace std;


static constexpr auto TIMER_INTERVAL = chrono::milliseconds(1000);


ADCBattery::ADCBattery(shared_ptr<RobotContext> context):
    m_initialized { false },
    m_timer { context->io() }
{

}

ADCBattery::~ADCBattery() 
{
    cleanup();
}


void ADCBattery::init() 
{
    m_timer.expires_after(TIMER_INTERVAL);
    switch (rc_model_category()) {
	case CATEGORY_BEAGLEBONE:
        m_timer.async_wait(bind(&ADCBattery::timer, this, _1));
        break;
    default:
        break;
    }
    m_initialized = true;
}


void ADCBattery::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = true;

    m_timer.cancel();
}


void ADCBattery::timer(boost::system::error_code error) 
{
    if (error!=boost::system::errc::success || !m_initialized) {
        return;
    }

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
    m_timer.async_wait(bind(&ADCBattery::timer, this, _1));
}

