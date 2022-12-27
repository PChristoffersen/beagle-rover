#include "software.h"

#include <boost/log/trivial.hpp>

#include "../types.h"



namespace Robot::Input {


SoftwareSource::SoftwareSource(std::string name, const Signals &signals) :
    AbstractSource { signals },
    m_initialized { false },
    m_name { name },
    m_direction { 0.0f },
    m_throttle { 0.0f },
    m_aux_x { 0.0f },
    m_aux_y { 0.0f }
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


void SoftwareSource::setAxis(float direction, float throttle, float aux_x, float aux_y)
{
    const guard lock(m_mutex);
    if (!m_enabled) 
        return;
    m_direction = std::clamp(direction, STEER_MIN, STEER_MAX);
    m_throttle = std::clamp(throttle, THROTTLE_MIN, THROTTLE_MAX);
    m_aux_x = std::clamp(aux_x, STEER_MIN, STEER_MAX);
    m_aux_y = std::clamp(aux_y, STEER_MIN, STEER_MAX);
    //BOOST_LOG_TRIVIAL(info) << *this << "  Dir: " << m_direction << " Throttle: " << m_throttle;

    _steer(m_direction, m_throttle, m_aux_x, m_aux_y);
    notify(NOTIFY_DEFAULT);
}

void SoftwareSource::setDirection(float value)
{
    setAxis(value, m_throttle, m_aux_x, m_aux_y);
}


void SoftwareSource::setThrottle(float value)
{
    setAxis(m_direction, value, m_aux_x, m_aux_y);
}


void SoftwareSource::setAuxX(float value)
{
    setAxis(m_direction, m_throttle, value, m_aux_y);
}


void SoftwareSource::setAuxY(float value)
{
    setAxis(m_direction, m_throttle, m_aux_x, value);
}



}