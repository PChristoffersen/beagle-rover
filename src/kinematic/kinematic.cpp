#include "kinematic.h"

#include <boost/log/trivial.hpp>

#include "../robotcontext.h"
#include "../motor/motorcontrol.h"
#include "../motor/motor.h"
#include "../motor/motorservo.h"
#include "../input/inputcontrol.h"

#include "controlscheme/idle.h"
#include "controlscheme/standard.h"
#include "controlscheme/spinning.h"
#include "controlscheme/balancing.h"
#include "controlscheme/passthrough.h"


using namespace std;

namespace Robot::Kinematic {

Kinematic::Kinematic(const std::shared_ptr<Robot::Context> &context) :
    m_initialized { false },
    m_context { context },
    m_steering_mode { SteeringMode::NONE },
    m_drive_mode { DriveMode::NONE },
    m_orientation { Orientation::NORTH }
{

}

Kinematic::~Kinematic() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}



void Kinematic::init(const std::shared_ptr<Robot::Motor::Control> &motor_control, const std::shared_ptr<Robot::Telemetry::Telemetry> &telemetry, const std::shared_ptr<::Robot::Input::Control> &input_control) 
{
    const guard lock(m_mutex);
    m_initialized = true;
    m_motor_control = motor_control;

    m_axis_connection = input_control->signals.steer.connect(::Robot::Input::SignalSteer::slot_type(&Kinematic::onSteer, this, _1, _2, _3, _4).track_foreign(shared_from_this()));
}


void Kinematic::cleanup() 
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    m_initialized = false;

    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;

    m_axis_connection.disconnect();

    if (m_control_scheme) {
        m_control_scheme->cleanup();
        m_control_scheme = nullptr;
    }
    m_motor_control.reset();
}



void Kinematic::setSteeringMode(SteeringMode mode) 
{
    const guard lock(m_mutex);
    if (mode==m_steering_mode)
        return;
    m_steering_mode = mode;

    BOOST_LOG_TRIVIAL(info) << "Kinematic steering: " << (int)mode;
    if (m_control_scheme) {
        m_control_scheme->updateSteeringMode(mode);
    }
}


void Kinematic::setDriveMode(DriveMode mode) 
{
    const guard lock(m_mutex);
    if (mode!=m_drive_mode || !m_control_scheme) {
        BOOST_LOG_TRIVIAL(info) << "Kinematic scheme: " << (int)mode;

        if (m_control_scheme) {
            m_control_scheme->cleanup();
            m_control_scheme = nullptr;
        }

        switch (mode) {
        case DriveMode::STANDARD:
            m_control_scheme = make_shared<ControlSchemeStandard>(shared_from_this());
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
            m_control_scheme->updateOrientation(m_orientation);
            m_control_scheme->updateSteeringMode(m_steering_mode);
            m_control_scheme->init();
        }

    }
}


void Kinematic::setOrientation(Orientation orientation)
{
    const guard lock(m_mutex);
    if (orientation==m_orientation)
        return;
    m_orientation = orientation;

    BOOST_LOG_TRIVIAL(info) << "Kinematic orientation: " << (int)orientation;

    if (m_control_scheme) {
        m_control_scheme->updateOrientation(orientation);
    }
}


void Kinematic::onSteer(double steering, double throttle, double aux_x, double aux_y) 
{
    BOOST_LOG_TRIVIAL(info) << "Kinematic onSteer " << steering << " " << throttle;
    if (m_control_scheme) {
        m_control_scheme->steer(steering, throttle, aux_x, aux_y);
    }
}


};