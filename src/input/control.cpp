#include "control.h"

#include <boost/log/trivial.hpp>

#include "sources/software.h"
#include "sources/rc.h"
#include "sources/gamepad.h"


namespace Robot::Input {

Control::Control(const std::shared_ptr<Robot::Context> &context) :
    m_input_source { InputSource::MANUAL },
    m_manual_source { std::make_unique<SoftwareSource>("Manual", signals) },
    m_web_source { std::make_unique<SoftwareSource>("WEB", signals) },
    #if ROBOT_HAVE_RC
    m_rc_source { std::make_unique<RCSource>(signals) },
    #endif
    m_gamepad_source { std::make_unique<GamepadSource>(context, signals) },
    m_active_source { m_manual_source.get() }
{
}

Control::~Control() 
{
    cleanup();
}


SoftwareInterface *Control::manual() const 
{
    return m_manual_source.get();
}

SoftwareInterface *Control::web() const
{
    return m_web_source.get();
}

#if ROBOT_HAVE_RC
void Control::init(const std::shared_ptr<Robot::RC::Receiver> &receiver)
#else
void Control::init()
#endif
{
    const guard lock(m_mutex);
    m_initialized = true;

    m_manual_source->init();
    m_web_source->init();
    #if ROBOT_HAVE_RC
    m_rc_source->init(receiver);
    #endif
    m_gamepad_source->init();

    m_manual_source->setEnabled(true);
    m_input_source = InputSource::MANUAL;
    m_active_source = m_manual_source.get();
}

void Control::cleanup()
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    m_initialized = false;

    m_input_source = InputSource::MANUAL;
    m_active_source = m_manual_source.get();

    m_manual_source->cleanup();
    m_web_source->cleanup();
    #if ROBOT_HAVE_RC
    m_rc_source->cleanup();
    #endif
    m_gamepad_source->cleanup();
}


void Control::setInputSource(InputSource input)
{
    const guard lock(m_mutex);
    if (input!=m_input_source) {
        BOOST_LOG_TRIVIAL(info) << "Input source: " << (int)input;

        m_active_source->setEnabled(false);

        switch (input) {
        case InputSource::RC:
            #if ROBOT_HAVE_RC
            m_active_source = m_rc_source.get();
            #else
            m_active_source = m_manual_source.get();
            #endif
            break;
        case InputSource::WEB:
            m_active_source = m_web_source.get();
            break;
        case InputSource::CONTROLLER:
            // TODO
            m_active_source = m_manual_source.get();
            break;
        case InputSource::MANUAL:
            m_active_source = m_manual_source.get();
            break;
        }

        m_active_source->setEnabled(true);
        m_input_source = input;

    }
}


};