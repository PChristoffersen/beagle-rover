#include "abstractcontrolscheme.h"

#include <boost/log/trivial.hpp>

#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "../types.h"


namespace Robot::Kinematic {


AbstractControlScheme::AbstractControlScheme(std::shared_ptr<Kinematic> kinematic) :
    m_initialized { false },
    m_motor_map { MOTOR_MAP_NORTH },
    m_context { kinematic->context() },
    m_motor_control { kinematic->motorControl() }
{
}


void AbstractControlScheme::updateOrientation(Orientation orientation) 
{
    const guard lock(m_mutex);
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
        m_motor_map = MOTOR_MAP_NORTH;
        break;
    }
}


};