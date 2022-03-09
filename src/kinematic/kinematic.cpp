#include "kinematic.h"

#include <boost/log/trivial.hpp>

#include <robotcontext.h>
#include <motor/control.h>
#include <motor/motor.h>
#include <motor/servo.h>
#include <input/control.h>

#include "controlscheme/idle.h"
#include "controlscheme/allwheel.h"
#include "controlscheme/frontwheel.h"
#include "controlscheme/rearwheel.h"
#include "controlscheme/skid.h"
#include "controlscheme/spinning.h"
#include "controlscheme/balancing.h"
#include "controlscheme/passthrough.h"


using namespace std::literals;


namespace Robot::Kinematic {


std::ostream &operator<<(std::ostream &os, const Orientation &orientation)
{
    switch (orientation) {
        case Orientation::NORTH:
            os << "NORTH";
            break;
        case Orientation::SOUTH:
            os << "SOUTH";
            break;
        case Orientation::EAST:
            os << "EAST";
            break;
        case Orientation::WEST:
            os << "WEST";
            break;
    }
    return os;
}



Kinematic::Kinematic(const std::shared_ptr<Robot::Context> &context) :
    m_initialized { false },
    m_context { context },
    m_drive_mode { DriveMode::NONE },
    m_orientation { Orientation::NORTH }
{

}

Kinematic::~Kinematic() 
{
    cleanup();
}



void Kinematic::init(const std::shared_ptr<Robot::Motor::Control> &motor_control, const std::shared_ptr<::Robot::LED::Control> &led_control, const std::shared_ptr<Robot::Telemetry::Telemetry> &telemetry, const std::shared_ptr<::Robot::Input::Control> &input_control) 
{
    const guard lock(m_mutex);
    m_initialized = true;
    m_motor_control = motor_control;
    m_led_control = led_control;
    m_telemetry = telemetry;

    m_control_scheme = std::make_shared<ControlSchemeIdle>(shared_from_this());
    m_drive_mode = DriveMode::NONE;

    m_axis_connection        = input_control->signals.steer.connect([&](auto d, auto f, auto ax, auto ay){ onSteer(d, f, ax, ay); });
    m_drive_mode_connection  = input_control->signals.drive_mode.connect([&](DriveMode drive_mode) { setDriveMode(drive_mode); });
    m_orientation_connection = input_control->signals.orientation.connect([&](Orientation orientation) { setOrientation(orientation); });
}


void Kinematic::cleanup() 
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    m_initialized = false;

    m_axis_connection.disconnect();
    m_drive_mode_connection.disconnect();
    m_orientation_connection.disconnect();

    if (m_control_scheme) {
        m_control_scheme->cleanup();
        m_control_scheme = nullptr;
    }
    m_motor_control.reset();
}



void Kinematic::setDriveMode(DriveMode mode) 
{
    const guard lock(m_mutex);
    if (mode!=m_drive_mode || !m_control_scheme) {
        //BOOST_LOG_TRIVIAL(info) << "Kinematic scheme: " << (int)mode;

        m_control_scheme->cleanup();
        m_control_scheme = nullptr;

        switch (mode) {
        case DriveMode::ALL_WHEEL:
            m_control_scheme = std::make_shared<ControlSchemeAllWheel>(shared_from_this());
            break;
        case DriveMode::FRONT_WHEEL:
            m_control_scheme = std::make_shared<ControlSchemeFrontWheel>(shared_from_this());
            break;
        case DriveMode::REAR_WHEEL:
            m_control_scheme = std::make_shared<ControlSchemeRearWheel>(shared_from_this());
            break;
        case DriveMode::SKID:
            m_control_scheme = std::make_shared<ControlSchemeSkid>(shared_from_this());
            break;
        case DriveMode::SPINNING:
            m_control_scheme = std::make_shared<ControlSchemeSpinning>(shared_from_this());
            break;
        #if ROBOT_HAVE_BALANCING
        case DriveMode::BALANCING:
            m_control_scheme = std::make_shared<ControlSchemeBalancing>(shared_from_this());
            break;
        #endif
        case DriveMode::PASSTHROUGH:
            m_control_scheme = std::make_shared<ControlSchemePassthrough>(shared_from_this());
            break;
        default:
            m_control_scheme = std::make_shared<ControlSchemeIdle>(shared_from_this());
            mode = DriveMode::NONE;
            break;
        }

        m_drive_mode = mode;
        m_control_scheme->updateOrientation(m_orientation);
        m_control_scheme->init();

        notify(NOTIFY_DEFAULT);
    }
}


void Kinematic::setOrientation(Orientation orientation)
{
    const guard lock(m_mutex);
    if (orientation==m_orientation)
        return;
  
    m_orientation = orientation;

    BOOST_LOG_TRIVIAL(info) << "Kinematic orientation: " << orientation;
    m_control_scheme->updateOrientation(orientation);

    notify(NOTIFY_DEFAULT);
}


void Kinematic::onSteer(float steering, float throttle, float aux_x, float aux_y) 
{
    BOOST_LOG_TRIVIAL(trace) << "Kinematic onSteer " << steering << " " << throttle;
    m_control_scheme->steer(steering, throttle, aux_x, aux_y);
}


}