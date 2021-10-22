
#include "kinematic.h"
#include "../robotcontext.h"
#include "../motor/motorcontrol.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"

#include "controlnormal.h"
#include "controlspinning.h"
#include "controlbalancing.h"

using namespace boost;


shared_ptr<Kinematic> Kinematic::create(shared_ptr<RobotContext> context, boost::shared_ptr<class MotorControl> motor_control, boost::shared_ptr<class Telemetry> telemetry) {
    return shared_ptr<Kinematic>(new Kinematic(context, motor_control, telemetry));
}


Kinematic::Kinematic(shared_ptr<RobotContext> context, boost::shared_ptr<class MotorControl> motor_control, boost::shared_ptr<class Telemetry> telemetry) :
    m_initialized(false),
    m_context(context),
    m_motor_control(motor_control),
    m_telemetry(telemetry),
    m_steering_mode(STEERING_NONE),
    m_drive_mode(DRIVE_NONE)
{

}

Kinematic::~Kinematic() {
    cleanup();
}



void Kinematic::init() {
    m_initialized = true;
}


void Kinematic::cleanup() {
    if (!m_initialized)
        return;
    
    m_initialized = false;
}



void Kinematic::setSteeringMode(SteeringMode mode) {
    const std::lock_guard<std::recursive_mutex> lock(m_mutex);
    switch (mode) {
    case STEERING_NONE:
        break;
    case STEERING_FRONT:
        break;
    case STEERING_REAR:
        break;
    case STEERING_4WD:
        break;
    case STEERING_SKID:
        break;
    case STEERING_BYPASS:
        break;
    }
    m_steering_mode = mode;
}


void Kinematic::setDriveMode(DriveMode mode) {
    const std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (mode!=m_drive_mode) {
        if (m_control_scheme) {
            m_control_scheme->cleanup();
            m_control_scheme.reset();
        }
        switch (mode) {
        case DRIVE_NONE:
            break;
        case DRIVE_NORMAL:
            m_control_scheme = ControlNormal::create(shared_from_this());
            break;
        case DRIVE_SPINNING:
            m_control_scheme = ControlSpinning::create(shared_from_this());
            break;
        case DRIVE_BALANCING:
            m_control_scheme = ControlBalancing::create(shared_from_this());
            break;
        }
        if (m_control_scheme) {
            m_control_scheme->init();
        }
        m_drive_mode = mode;
    }
}

