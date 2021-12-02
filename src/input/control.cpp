#include "control.h"

#include <boost/log/trivial.hpp>

#include "sources/software.h"
#include "sources/rc.h"
#include "sources/gamepad.h"


using namespace std;

namespace Robot::Input {

Control::Control(const shared_ptr<Robot::Context> &context) :
    m_input_source { InputSource::MANUAL },
    m_manual_source { make_unique<SoftwareSource>("Manual", signals) },
    m_web_source { make_unique<SoftwareSource>("WEB", signals) },
    m_rc_source { make_unique<RCSource>(signals) },
    m_gamepad_source { make_unique<GamepadSource>(context, signals) },
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


void Control::init(const std::shared_ptr<Robot::RC::Receiver> &receiver)
{
    const guard lock(m_mutex);
    m_initialized = true;

    m_manual_source->init();
    m_web_source->init();
    m_rc_source->init(receiver);
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
    m_rc_source->cleanup();
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
            m_active_source = m_rc_source.get();
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