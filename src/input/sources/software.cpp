#include "software.h"

#include <boost/log/trivial.hpp>

#include "../types.h"



namespace Robot::Input {


SoftwareSource::SoftwareSource(std::string name, const Signals &signals) :
    AbstractSource { signals },
    m_initialized { false },
    m_enabled { false },
    m_name { name }
{

}


SoftwareSource::~SoftwareSource()
{
    cleanup();
}



void SoftwareSource::init()
{
    const guard lock(m_mutex);
    m_initialized = true;
}


void SoftwareSource::cleanup()
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    m_initialized = false;
}


void SoftwareSource::setEnabled(bool enabled)
{
    const guard lock(m_mutex);
    if (enabled==m_enabled)
        return;
    m_enabled = enabled;
}

void SoftwareSource::steer(double steer, double throttle, double aux_x, double aux_y)
{
    const guard lock(m_mutex);
    if (!m_enabled) 
        return;
    steer = std::clamp(steer, STEER_MIN, STEER_MAX);
    throttle = std::clamp(throttle, THROTTLE_MIN, THROTTLE_MAX);
    aux_x = std::clamp(aux_x, STEER_MIN, STEER_MAX);
    aux_y = std::clamp(aux_y, STEER_MIN, STEER_MAX);
    BOOST_LOG_TRIVIAL(trace) << *this << "  Steer: " << steer << " Throttle: " << throttle;

    m_signals.steer(steer, throttle, aux_x, aux_y);
}


};