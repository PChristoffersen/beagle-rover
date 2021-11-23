#include "kinematic.h"

#include <boost/log/trivial.hpp>

#include "../robotcontext.h"
#include "../motor/motorcontrol.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"

#include "controlscheme/idle.h"
#include "controlscheme/normal.h"
#include "controlscheme/spinning.h"
#include "controlscheme/balancing.h"
#include "controlscheme/passthrough.h"


using namespace std;

namespace Robot::Kinematic {

Kinematic::Kinematic(const std::shared_ptr<Robot::Context> &context, const std::shared_ptr<Robot::Motor::Control> &motor_control, const std::shared_ptr<Robot::Telemetry::Telemetry> &telemetry, const std::shared_ptr<Robot::RC::Receiver> &receiver) :
    m_initialized { false },
    m_context { context },
    m_motor_control { motor_control },
    m_rc_receiver { receiver },
    m_steering_mode { SteeringMode::NONE },
    m_drive_mode { DriveMode::NONE }
{

}

Kinematic::~Kinematic() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}



void Kinematic::init() 
{
    m_initialized = true;
}


void Kinematic::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;

    if (m_control_scheme) {
        m_control_scheme->cleanup();
        m_control_scheme = nullptr;
    }
}



void Kinematic::setSteeringMode(SteeringMode mode) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    switch (mode) {
    case SteeringMode::NONE:
        break;
    case SteeringMode::FRONT:
        break;
    case SteeringMode::REAR:
        break;
    case SteeringMode::ALL:
        break;
    case SteeringMode::SKID:
        break;
    }
    m_steering_mode = mode;
}


void Kinematic::setDriveMode(DriveMode mode) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (mode!=m_drive_mode || !m_control_scheme) {
        BOOST_LOG_TRIVIAL(info) << "Kinematic scheme: " << (int)m_drive_mode;

        if (m_control_scheme) {
            m_control_scheme->cleanup();
            m_control_scheme = nullptr;
        }

        switch (mode) {
        case DriveMode::NORMAL:
            m_control_scheme = make_shared<ControlSchemeNormal>(shared_from_this());
            break;
        case DriveMode::SPINNING:
            m_control_scheme = make_shared<ControlSchemeSpinning>(shared_from_this());
            break;
        case DriveMode::BALANCING:
            m_control_scheme = make_shared<ControlSchemeBalancing>(shared_from_this());
            break;
        case DriveMode::PASSTHROUGH:
            m_control_scheme = make_shared<ControlSchemePassthrough>(shared_from_this());
            break;
        default:
            m_control_scheme = make_shared<ControlSchemeIdle>(shared_from_this());
            break;
        }

        m_drive_mode = mode;

        if (m_control_scheme) {
            m_control_scheme->init();
        }

    }
}

};