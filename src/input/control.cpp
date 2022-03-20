#include "control.h"

#include <boost/log/trivial.hpp>

#include <rc/inputsource.h>
#include "sources/software.h"
#include "sources/gamepad.h"

namespace Robot::Input {


std::ostream &operator<<(std::ostream &os, const InputSource &input_source)
{
    switch (input_source) {
        case InputSource::MANUAL:
            os << "MANUAL";
            break;
        case InputSource::RC:
            os << "RC";
            break;
        case InputSource::WEB:
            os << "WEB";
            break;
        case InputSource::CONTROLLER:
            os << "CONTROLLER";
            break;
    }
    return os;
}



Control::Control(const std::shared_ptr<Robot::Context> &context) :
    m_axis_source { InputSource::MANUAL },
    m_kinematic_source { InputSource::MANUAL },
    m_led_source { InputSource::MANUAL },
    m_manual_source { std::make_shared<SoftwareSource>("Manual", signals) },
    m_web_source { std::make_shared<SoftwareSource>("WEB", signals) },
    m_gamepad_source { std::make_unique<GamepadSource>(context, signals) }
{
}

Control::~Control() 
{
    cleanup();
}


std::shared_ptr<SoftwareInterface> Control::manual() const 
{
    return m_manual_source;
}

std::shared_ptr<SoftwareInterface> Control::web() const
{
    return m_web_source;
}

void Control::init(const std::shared_ptr<Robot::RC::Receiver> &receiver)
{
    const guard lock(m_mutex);
    m_initialized = true;

    m_manual_source->init();
    m_web_source->init();

    #if ROBOT_HAVE_RC
    if (receiver) {
        m_rc_source = std::make_unique<Robot::RC::InputSource>(signals);
        m_rc_source->init(receiver);
    }
    #endif
    m_gamepad_source->init();

    m_manual_source->setEnabled(true);
    m_axis_source = InputSource::MANUAL;
    m_kinematic_source = InputSource::MANUAL;
    m_led_source = InputSource::MANUAL;
}

void Control::cleanup()
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    m_initialized = false;

    m_axis_source = InputSource::MANUAL;
    m_kinematic_source = InputSource::MANUAL;
    m_led_source = InputSource::MANUAL;

    m_manual_source->cleanup();
    m_web_source->cleanup();
    if (m_rc_source) {
        m_rc_source->cleanup();
        m_rc_source.reset();
    }
    m_gamepad_source->cleanup();
}



void Control::setAxisSource(InputSource input)
{
    const guard lock(m_mutex);
    if (input==m_axis_source) {
        BOOST_LOG_TRIVIAL(info) << "Input source: " << input;

        auto oldSource = findSource(m_axis_source);
        auto newSource = findSource(input);
        if (oldSource!=newSource) {
            oldSource->setEnabled(false);
            m_axis_source = input;
            newSource->setEnabled(true);
        }
        else if (newSource!=nullptr) {
            m_axis_source = input;
        }

        notify(NOTIFY_DEFAULT);
    }
}


void Control::setKinematicSource(InputSource input)
{
    const guard lock(m_mutex);
    if (input!=m_kinematic_source) {
        BOOST_LOG_TRIVIAL(info) << "Kinematic source: " << input;

        auto oldSource = findSource(m_kinematic_source);
        auto newSource = findSource(input);
        if (oldSource!=newSource) {
            oldSource->setEnabledKinematic(false);
            m_kinematic_source = input;
            newSource->setEnabledKinematic(true);
        }
        else if (newSource!=nullptr) {
            m_kinematic_source = input;
        }

        notify(NOTIFY_DEFAULT);
    }
}


void Control::setLedSource(InputSource input)
{
    const guard lock(m_mutex);
    if (input!=m_led_source) {
        BOOST_LOG_TRIVIAL(info) << "LED source: " << input;

        auto oldSource = findSource(m_led_source);
        auto newSource = findSource(input);
        if (oldSource!=newSource) {
            oldSource->setEnabledLED(false);
            m_led_source = input;
            newSource->setEnabledLED(true);
        }
        else if (newSource!=nullptr) {
            m_led_source = input;
        }

        notify(NOTIFY_DEFAULT);
    }
}



Source *Control::findSource(InputSource input)
{
    switch (input) {
    case InputSource::RC:
        return m_rc_source.get();
        break;
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