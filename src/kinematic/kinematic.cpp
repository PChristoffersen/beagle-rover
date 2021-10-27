#include <boost/log/trivial.hpp>

#include "kinematic.h"
#include "../robotcontext.h"
#include "../motor/motorcontrol.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"

#include "controlidle.h"
#include "controlnormal.h"
#include "controlspinning.h"
#include "controlbalancing.h"


using namespace std;


Kinematic::Kinematic(shared_ptr<RobotContext> context, shared_ptr<class MotorControl> motor_control, shared_ptr<class Telemetry> telemetry) :
    m_initialized { false },
    m_context { context },
    m_motor_control { motor_control },
    m_telemetry { telemetry },
    m_steering_mode { SteeringMode::NONE },
    m_drive_mode { DriveMode::NONE }
{

}

Kinematic::~Kinematic() 
{
    cleanup();
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
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
        m_control_scheme.reset();
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
    case SteeringMode::PASSTHROUGH:
        break;
    }
    m_steering_mode = mode;
}


void Kinematic::setDriveMode(DriveMode mode) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (mode!=m_drive_mode || !m_control_scheme) {
        BOOST_LOG_TRIVIAL(info) << "Control scheme: " << (int)m_drive_mode;
        if (m_control_scheme) {
            BOOST_LOG_TRIVIAL(info) << ">>>>>";
            m_control_scheme->cleanup();
            m_control_scheme.reset();
            BOOST_LOG_TRIVIAL(info) << "<<<<<";
        }
        switch (mode) {
        case DriveMode::NORMAL:
            m_control_scheme = make_shared<ControlNormal>(shared_from_this());
            break;
        case DriveMode::SPINNING:
            m_control_scheme = make_shared<ControlSpinning>(shared_from_this());
            break;
        case DriveMode::BALANCING:
            m_control_scheme = make_shared<ControlBalancing>(shared_from_this());
            break;
        default:
            m_control_scheme = make_shared<ControlIdle>(shared_from_this());
            break;
        }
        if (m_control_scheme) {
            m_control_scheme->init();
        }
        m_drive_mode = mode;
    }
}


