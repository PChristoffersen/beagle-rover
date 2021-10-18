#include <typeinfo>
#include <boost/log/trivial.hpp>
#include <robotcontrol-ext.h>

#include "telemetry.h"
#include "telemetrytypes.h"
#include "battery.h"
#include "../robotcontext.h"

using namespace boost;
using namespace boost::asio;

#define TELEMETRY_BATTERY 0x0300



Telemetry::Telemetry(shared_ptr<RobotContext> context) :
    m_context(context),
    m_battery(new Battery(context))
{
}


void Telemetry::init() {
    m_battery->init();
    m_battery_connection = m_battery->sig_event.connect(boost::bind(&Telemetry::process, this, _1));
}


void Telemetry::cleanup() {
    m_battery_connection.disconnect();
    m_battery->cleanup();
}


void Telemetry::send(uint16_t appId, uint32_t data) {
    //rc_ext_fbus_send_telemetry(appId, data);
}


void Telemetry::process(const TelemetryEvent &event) {
    if (typeid(event)==typeid(TelemetryEventBattery)) {
        processBattery((const TelemetryEventBattery&)event);
    }
}

void Telemetry::processBattery(const TelemetryEventBattery &event) {
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
