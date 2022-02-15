#include "robotcontrolbattery.h"

#if ROBOT_HAVE_ROBOTCONTROL_BATTERY

#include <iostream>
#include <boost/log/trivial.hpp>

#include <robotconfig.h>
#include <robotcontext.h>
#include "../types.h"
#include "../events.h"
#include "../telemetry.h"

using namespace std::literals;

namespace Robot::Telemetry {

static constexpr auto TIMER_INTERVAL { 500ms };
static const std::string SOURCE_NAME { "battery" };

static constexpr auto CELL_COUNT { 2.0 }; // Number of battery cells 
static constexpr auto CELL_MAX  { 4.25 }; // set 1er than actual to detect num cells
static constexpr auto CELL_FULL	{ 3.90 }; // minimum V to consider battery full
static constexpr auto CELL_75   { 3.80 }; // minimum V to consider battery 75%
static constexpr auto CELL_50   { 3.60 }; // minimum V to consider battery 50%
static constexpr auto CELL_25   { 3.25 }; // minimum V to consider battery 25%
static constexpr auto CELL_DIS  { 2.70 }; // Threshold for detecting disconnected battery
static constexpr auto V_CHG_DETECT { 4.15 }; // above this assume finished charging
static constexpr auto V_JACK_THRES   { 10.0 }; // above this assume DC connected

static constexpr auto FILTER_RESET_THRES { 2.0 };
static constexpr auto FILTER_SAMPLES { 6 }; // average over 6 samples

RobotControlBattery::RobotControlBattery(const std::shared_ptr<Robot::Context> &context):
    AbstractSource { context },
    m_initialized { false },
    m_timer { context->io() }, 
    m_pack_filter { rc_filter_empty() },
    m_jack_filter { rc_filter_empty() },
    m_pack_voltage { 0.0 },
    m_jack_voltage { 0.0 }

{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;

}

RobotControlBattery::~RobotControlBattery() 
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    cleanup();

    rc_filter_free(&m_pack_filter);
    rc_filter_free(&m_jack_filter);
}


void RobotControlBattery::init(const std::shared_ptr<Telemetry> &telemetry) 
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    AbstractSource::init(telemetry);

    m_initialized = true;

    std::chrono::duration<double> dur { TIMER_INTERVAL };

    // Initialized battery filter
    m_pack_voltage = rc_adc_batt();
    if (rc_filter_moving_average(&m_pack_filter, FILTER_SAMPLES, dur.count())) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Failed to create battery filter"));
	}
    rc_filter_prefill_outputs(&m_pack_filter, m_pack_voltage);
	rc_filter_prefill_inputs(&m_pack_filter, m_pack_voltage);

    // Initialized dc jack filter
    m_jack_voltage = rc_adc_dc_jack();
	if (rc_filter_moving_average(&m_jack_filter, FILTER_SAMPLES, dur.count())){
        BOOST_THROW_EXCEPTION(std::runtime_error("Failed to create DC filter"));
	}
	rc_filter_prefill_outputs(&m_jack_filter, m_jack_voltage);
	rc_filter_prefill_inputs(&m_jack_filter, m_jack_voltage);

    // Setup battery timer
    m_timer.expires_after(TIMER_INTERVAL);
    timer_setup();

}


void RobotControlBattery::cleanup() 
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    if (!m_initialized)
        return;
    m_initialized = false;

    m_timer.cancel();
    AbstractSource::cleanup();
}


void RobotControlBattery::timer_setup() {
    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(
        [self_ptr=weak_from_this()] (boost::system::error_code error) {
            if (auto self = self_ptr.lock()) { 
                self->timer(error); 
            }
        }
    );
}

void RobotControlBattery::timer(boost::system::error_code error) 
{
    if (error!=boost::system::errc::success || !m_initialized) {
        return;
    }

    auto v_pack = rc_adc_batt();
    auto v_jack = rc_adc_dc_jack();

    // if there is a sudden jump due to connection or disconnection
	// reset the filters
    if(std::abs(v_pack-m_pack_voltage)>FILTER_RESET_THRES){
	    rc_filter_prefill_outputs(&m_pack_filter, v_pack);
		rc_filter_prefill_inputs(&m_pack_filter, v_pack);
	}
    if(std::abs(v_jack-m_jack_voltage)>FILTER_RESET_THRES){
		rc_filter_prefill_outputs(&m_jack_filter, m_jack_voltage);
		rc_filter_prefill_inputs(&m_jack_filter, m_jack_voltage);
	}

	// march moving average filter
	m_pack_voltage = rc_filter_march(&m_pack_filter, v_pack);
	m_jack_voltage = rc_filter_march(&m_jack_filter, v_jack);

    auto cell_voltage { m_pack_voltage/CELL_COUNT };
    auto charging { false };
    auto on_battery { cell_voltage>0.0 };
    // check charging condition
	if(m_jack_voltage>V_JACK_THRES) {
        on_battery = false;
        if (cell_voltage<V_CHG_DETECT) {
		    charging = true;
        }
	}

    // Determine battery charge
    float percent { 0.0f };
	if(cell_voltage<CELL_DIS)       percent = 0.00f; // Fully discharged
	else if(cell_voltage>CELL_FULL)	percent = 1.00f; // Battery full
	else if(cell_voltage>CELL_75)	percent = 0.75f; // 75%
	else if(cell_voltage>CELL_50)	percent = 0.50f; // 50%
	else if(cell_voltage>CELL_25)	percent = 0.25f; // 25%
    else                            percent = 0.00f; // Critical

    // Fill and send event
    EventBattery event { SOURCE_NAME };
    event.battery_id = 0x00;
    event.charging = charging;
    event.on_battery = on_battery;
    event.jack_voltage = m_jack_voltage;
    event.percent = percent;
    event.voltage = m_pack_voltage;
    event.cell_voltage.push_back(cell_voltage);
    event.cell_voltage.push_back(cell_voltage);
    sendEvent(event);

    timer_setup();
}

}

#endif