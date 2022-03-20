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
    WithStrand { context->io() },
    m_initialized { false },
    m_context { context },
    m_drive_mode { DriveMode::NONE },
    m_orientation { Orientation::NORTH }
{
    #if ROBOT_HAVE_BALANCING
    ControlSchemeBalancing::registerProperties(context);
    #endif
}

Kinematic::~Kinematic() 
{
    cleanup();
}



void Kinematic::init(const std::shared_ptr<Robot::Motor::Control> &motor_control, const std::shared_ptr<::Robot::LED::Control> &led_control, const std::shared_ptr<Robot::Telemetry::Telemetry> &telemetry, const std::shared_ptr<::Robot::Input::Control> &input_control) 
{
    m_initialized = true;
    m_motor_control = motor_control;
    m_led_control = led_control;
    m_telemetry = telemetry;

    m_odometer = 0;
    m_odometer_base = 0;
    m_motor_update_connection = motor_control->sig_motor.connect([this](auto &motors) { onMotorUpdate(motors); });

    m_control_scheme = std::make_shared<ControlSchemeIdle>(shared_from_this());
    m_drive_mode = DriveMode::NONE;

    m_axis_connection        = input_control->signals.steer.connect([this](auto d, auto f, auto ax, auto ay){ onSteer(d, f, ax, ay); });
    m_drive_mode_connection  = input_control->signals.drive_mode.connect([this](DriveMode drive_mode) { setDriveMode(drive_mode); });
    m_orientation_connection = input_control->signals.orientation.connect([this](Orientation orientation) { setOrientation(orientation); });
}


void Kinematic::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = false;

    m_axis_connection.disconnect();
    m_drive_mode_connection.disconnect();
    m_orientation_connection.disconnect();
    m_motor_update_connection.disconnect();

    if (m_control_scheme) {
        m_control_scheme->cleanup();
        m_control_scheme = nullptr;
    }
    m_motor_control.reset();
    m_led_control.reset();
    m_telemetry.reset();
}



void Kinematic::setDriveMode(DriveMode mode) 
{
    const guard lock(m_mutex);
    if (mode==m_drive_mode) 
        return;

    m_drive_mode = mode;

    dispatch([this, mode]{
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
        default:
            m_control_scheme = std::make_shared<ControlSchemeIdle>(shared_from_this());
            m_drive_mode = DriveMode::NONE;
            break;
        }

        m_control_scheme->updateOrientation(m_orientation);
        m_control_scheme->init();

        notify(NOTIFY_DEFAULT);
    });
}


void Kinematic::setOrientation(Orientation orientation)
{
    const guard lock(m_mutex);
    if (orientation==m_orientation)
        return;
  
    m_orientation = orientation;

    dispatch([this,orientation]{
        m_odometer_base = getOdometer();
        for (const auto &motor : m_motor_control->getMotors()) {
            motor->resetOdometer();
        }

        switch (orientation) {
            case Orientation::NORTH:
                m_motor_map = MOTOR_MAP_NORTH;
                break;
            case Orientation::SOUTH:
                m_motor_map = MOTOR_MAP_SOUTH;
                break;
            case Orientation::EAST:
                m_motor_map = MOTOR_MAP_EAST;
                break;
            case Orientation::WEST:
                m_motor_map = MOTOR_MAP_WEST;
                break;
        }

        BOOST_LOG_TRIVIAL(info) << "Kinematic orientation: " << orientation;
        m_control_scheme->updateOrientation(orientation);

        notify(NOTIFY_DEFAULT);
    });
}


void Kinematic::resetOdometer()
{
    const guard lock(m_mutex);
    dispatch([this]{
        m_odometer = 0;
        m_odometer_base = 0;
        for (const auto &motor : m_motor_control->getMotors()) {
            motor->resetOdometer();
        }
        notify(NOTIFY_TELEMETRY);
    });
}



void Kinematic::onMotorUpdate(const ::Robot::Motor::MotorList &motors)
{
    dispatch([this]{
        const guard lock(m_mutex);
        odometer_type odometer = 0;
        const auto &motors = m_motor_control->getMotors();
        for (size_t i=0u; i<motors.size(); i++) {
            if (m_motor_map[i].invert_duty) {
                odometer += -motors[i]->getOdometer();
            }
            else {
                odometer += motors[i]->getOdometer();
            }
        }
        odometer /= motors.size();
        odometer += m_odometer_base;

        if (odometer!=m_odometer) {
            m_odometer = odometer;
            notify(NOTIFY_TELEMETRY);
        }
    });
}



void Kinematic::onSteer(float steering, float throttle, float aux_x, float aux_y) 
{
    BOOST_LOG_TRIVIAL(trace) << "Kinematic onSteer " << steering << " " << throttle;
    dispatch([&]{
        m_control_scheme->steer(steering, throttle, aux_x, aux_y);
    });
}


}