#ifndef _ROBOT_HARDWARE_BEAGLEBONE_SERVOPOWER_H_
#define _ROBOT_HARDWARE_BEAGLEBONE_SERVOPOWER_H_

#include <robotconfig.h>

#if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE

#include <boost/log/trivial.hpp>
#include "../abstractpower.h"

namespace Robot::Hardware::Beaglebone {

    class ServoPower : public Robot::Hardware::AbstractPower {
        protected:
            virtual void enable() override;
            virtual void disable() override;
    };

}

#endif
#endif
