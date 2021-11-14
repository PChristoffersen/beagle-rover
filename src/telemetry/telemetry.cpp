#include "telemetry.h"

#include <typeinfo>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "telemetrytypes.h"
#include "adcbattery.h"
#include "rcmpu.h"
#include "../robotcontext.h"


using namespace std;


static constexpr uint16_t TELEMETRY_BATTERY { 0x0300 };


Telemetry::Telemetry(shared_ptr<RobotContext> context) :
    m_initialized { false }
{
    switch (rc_model()) {
    case MODEL_BB_BLUE:
        m_sources.push_back(make_shared<ADCBattery>(context));
        break;
    default:
        break;
    }
}


Telemetry::~Telemetry() 
{
    cleanup();
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void Telemetry::init() 
{
    for (auto &source : m_sources) {
        source->init();
        m_source_connections.push_back(source->sig_event.connect([&](const auto &e){ process(e); }));
    }
    m_initialized = true;
}


void Telemetry::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = false;
    
    for (auto &connection : m_source_connections) {
        connection.disconnect();
    }
    m_source_connections.clear();
    for (auto &source : m_sources) {
        source->cleanup();
    }
}


void Telemetry::send(uint16_t appId, uint32_t data) 
{
    //BOOST_LOG_TRIVIAL(trace) << "Send";
    rc_ext_fbus_send_telemetry(appId, data);
}


void Telemetry::process(const TelemetryEvent &event) 
{
    if (const auto ev = dynamic_cast<const TelemetryEventBattery*>(&event)) {
        processBattery(*ev);
    }
}

void Telemetry::processBattery(const TelemetryEventBattery &event) 
{
    auto n_cells = event.cell_voltage.size();
    for (size_t i=0; i<n_cells; i+=2) {
        uint32_t cv1 = event.cell_voltage[i]*500;
        uint32_t cv2 = 0;
        
        if (i+1<n_cells) {
            cv2 = event.cell_voltage[i]*500;
        }
        
        uint32_t data = ((uint32_t) cv1 & 0x0fff) << 20 | ((uint32_t) cv2 & 0x0fff) << 8 | n_cells << 4 | event.battery_id;

        send(TELEMETRY_BATTERY + i/2, data);
    }
    sig_event(event);
}
