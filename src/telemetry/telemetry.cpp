#include "telemetry.h"

#include <typeinfo>
#include <boost/log/trivial.hpp>

#include <robotcontrol.h>
#include <robotcontrolext.h>

#include "telemetrytypes.h"
#include "sources/adcbattery.h"
#include "sources/rcmpu.h"
#include "../robotcontext.h"


using namespace std;

namespace Robot::Telemetry {

Telemetry::Telemetry(const shared_ptr<Robot::Context> &context, const shared_ptr<Robot::RC::Receiver> &receiver) :
    m_initialized { false },
    m_receiver { receiver }
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
        source->init(shared_from_this());
    }
    m_initialized = true;
}


void Telemetry::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = false;
    
    for (auto &source : m_sources) {
        source->cleanup();
    }
}


void Telemetry::process(const Event &event) 
{
    sig_event(event);
}
    
};
