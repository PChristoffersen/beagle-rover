#include "control.h"

#include <boost/log/trivial.hpp>

#include "sources/software.h"
#include "sources/rc.h"
#include "sources/gamepad.h"


namespace Robot::Input {

Control::Control(const std::shared_ptr<Robot::Context> &context) :
    m_source { InputSource::MANUAL },
    m_kinematic_source { InputSource::MANUAL },
    m_led_source { InputSource::MANUAL },
    m_manual_source { std::make_unique<SoftwareSource>("Manual", signals) },
    m_web_source { std::make_unique<SoftwareSource>("WEB", signals) },
    #if ROBOT_HAVE_RC
    m_rc_source { std::make_unique<RCSource>(signals) },
    #endif
    m_gamepad_source { std::make_unique<GamepadSource>(context, signals) }
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
    m_source = InputSource::MANUAL;
    m_kinematic_source = InputSource::MANUAL;
    m_led_source = InputSource::MANUAL;
}

void Control::cleanup()
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    m_initialized = false;

    m_source = InputSource::MANUAL;
    m_kinematic_source = InputSource::MANUAL;
    m_led_source = InputSource::MANUAL;

    m_manual_source->cleanup();
    m_web_source->cleanup();
    #if ROBOT_HAVE_RC
    m_rc_source->cleanup();
    #endif
    m_gamepad_source->cleanup();
}



void Control::setSource(InputSource input)
{
    const guard lock(m_mutex);
    if (input!=m_source) {
        BOOST_LOG_TRIVIAL(info) << "Input source: " << (int)input;

        auto oldSource = findSource(m_source);
        auto newSource = findSource(input);
        if (oldSource!=newSource) {
            oldSource->setEnabled(false);
            m_source = input;
            newSource->setEnabled(true);
        }
        else {
            m_source = input;
        }

        notify(NOTIFY_DEFAULT);
    }
}


void Control::setKinematicSource(InputSource input)
{
    const guard lock(m_mutex);
    if (input!=m_kinematic_source) {
        BOOST_LOG_TRIVIAL(info) << "Kinematic source: " << (int)input;

        #if 0
        auto oldSource = findSource(m_kinematic_source);
        auto newSource = findSource(input);
        if (oldSource!=newSource) {
            oldSource->setEnabled(false);
            m_kinematic_source = input;
            newSource->setEnabled(true);
        }
        else {
            m_kinematic_source = input;
        }
        #else
        m_kinematic_source = input;
        #endif

        notify(NOTIFY_DEFAULT);
    }
}


void Control::setLedSource(InputSource input)
{
    const guard lock(m_mutex);
    if (input!=m_led_source) {
        BOOST_LOG_TRIVIAL(info) << "LED source: " << (int)input;

        #if 0
        auto oldSource = findSource(m_led_source);
        auto newSource = findSource(input);
        if (oldSource!=newSource) {
            oldSource->setEnabled(false);
            m_led_source = input;
            newSource->setEnabled(true);
        }
        else {
            m_led_source = input;
        }
        #else
        m_led_source = input;
        #endif

        notify(NOTIFY_DEFAULT);
    }
}



Source *Control::findSource(InputSource input)
{
    switch (input) {
    #if ROBOT_HAVE_RC
    case InputSource::RC:
        return m_rc_source.get();
        break;
    #endif
    case InputSource::WEB:
        return m_web_source.get();
        break;
    case InputSource::CONTROLLER:
        // TODO
        return m_manual_source.get();
        break;
    case InputSource::MANUAL:
    default:
        return m_manual_source.get();
        break;
    }

}

}