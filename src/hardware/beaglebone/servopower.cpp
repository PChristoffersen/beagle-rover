#include "servopower.h"

#include <boost/log/trivial.hpp>

#if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
#include <robotcontrol.h>

namespace Robot::Hardware::Beaglebone {

void ServoPower::enable()
{
    rc_servo_power_rail_en(1);
}

void ServoPower::disable() 
{
    rc_servo_power_rail_en(0);
}


}

#endif
