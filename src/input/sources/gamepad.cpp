#include "gamepad.h"

#include <boost/log/trivial.hpp>


namespace Robot::Input {

GamepadSource::GamepadSource(const std::shared_ptr<::Robot::Context> &context, const Signals &signals) :
    AbstractSource { signals },
    m_initialized { false },
    m_enabled { false }
{
    
}

GamepadSource::~GamepadSource() 
{
    cleanup();
}


void GamepadSource::init()
{
    const guard lock(m_mutex);
    m_initialized = true;
}


void GamepadSource::cleanup()
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    m_initialized = false;
}


void GamepadSource::setEnabled(bool enabled)
{
    const guard lock(m_mutex);
    if (enabled==m_enabled)
        return;
    m_enabled = enabled;
    BOOST_LOG_TRIVIAL(info) << "SoftSource enabled: " << enabled;
}


}