#include "motorpower.h"

#include <boost/log/trivial.hpp>

#if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE

namespace Robot::Hardware::Beaglebone {

void MotorPower::enable()
{
    rc_motor_standby(1);
}

void MotorPower::disable() 
{
    rc_motor_standby(0);
}


};

#endif
