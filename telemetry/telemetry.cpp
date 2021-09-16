
#include "telemetry.h"
#include "battery.h"

using namespace boost::asio;


Telemetry::Telemetry(io_context &io) :
    m_battery(new Battery(io, this->shared_from_this()))
{

}


void Telemetry::init() {
    m_battery->init();
    Component::init();
}


void Telemetry::cleanup() {
    m_battery->cleanup();
    Component::cleanup();
}


void Telemetry::send(uint16_t appId, uint32_t data) {
    sig_data(appId, data);
    //rc_ext_fbus_send_telemetry(appId, data);
}