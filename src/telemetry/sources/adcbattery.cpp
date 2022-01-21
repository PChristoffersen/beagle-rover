#include "adcbattery.h"

#include <iostream>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <robotcontext.h>
#include "../types.h"
#include "../events.h"
#include "../telemetry.h"

using namespace std::literals;

namespace Robot::Telemetry {

static constexpr auto TIMER_INTERVAL { 2s };
static const std::string SOURCE_NAME { "battery" };


ADCBattery::ADCBattery(const std::shared_ptr<Robot::Context> &context):
    AbstractSource { context },
    m_initialized { false },
    m_timer { context->io() }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;

}

ADCBattery::~ADCBattery() 
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    cleanup();
}


void ADCBattery::init(const std::shared_ptr<Telemetry> &telemetry) 
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    AbstractSource::init(telemetry);

    m_initialized = true;

    m_timer.expires_after(TIMER_INTERVAL);
    timer_setup();

}


void ADCBattery::cleanup() 
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    if (!m_initialized)
        return;
    m_initialized = false;

    m_timer.cancel();
    AbstractSource::cleanup();
}


void ADCBattery::timer_setup() {
    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(
        [self_ptr=weak_from_this()] (boost::system::error_code error) {
            if (auto self = self_ptr.lock()) { 
                self->timer(error); 
            }
        }
    );
}

void ADCBattery::timer(boost::system::error_code error) 
{
    if (error!=boost::system::errc::success || !m_initialized) {
        return;
    }

    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    auto pack_voltage = rc_adc_batt();
    #else
    auto pack_voltage = 0.0;
    #endif

    if (pack_voltage<=0.0) {
        return;
    }
    
    EventBattery event { SOURCE_NAME };
    event.battery_id = 0x00;
    event.voltage = pack_voltage;
    event.cell_voltage.push_back(pack_voltage/2.0f);
    event.cell_voltage.push_back(pack_voltage/2.0f);

    send(event);

    timer_setup();
}


};