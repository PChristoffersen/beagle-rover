#include "abstractcontrolscheme.h"

#include <boost/log/trivial.hpp>

#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"


namespace Robot::Kinematic {


AbstractControlScheme::AbstractControlScheme(std::shared_ptr<Kinematic> kinematic) :
    m_initialized { false },
    m_orientation { Orientation::NORTH },
    m_orientation_reverse { false },
    m_motor_map { MOTOR_MAP_NORTH },
    m_context { kinematic->context() },
    m_motor_control { kinematic->motorControl() },
    m_last_steering { 0.0f },
    m_last_throttle { 0.0f },
    m_last_aux_x { 0.0f },
    m_last_aux_y { 0.0f }
{
}


void AbstractControlScheme::updateOrientation(Orientation orientation) 
{
    BOOST_LOG_TRIVIAL(info) << "UpdateOrientation: " << orientation;
    const guard lock(m_mutex);
    if (m_orientation!=orientation) {
        m_orientation = orientation;
        switch (orientation) {
        case Orientation::NORTH:
            m_motor_map = MOTOR_MAP_NORTH;
            m_orientation_reverse = false;
            break;
        case Orientation::SOUTH:
            m_motor_map = MOTOR_MAP_SOUTH;
            m_orientation_reverse = true;
            break;
        case Orientation::WEST:
            m_orientation_reverse = false;
            m_motor_map = MOTOR_MAP_WEST;
            break;
        case Orientation::EAST:
            m_orientation_reverse = true;
            m_motor_map = MOTOR_MAP_EAST;
            break;
        }
        if (m_initialized) {
            orientationUpdated(orientation);
        }
    }
}


}