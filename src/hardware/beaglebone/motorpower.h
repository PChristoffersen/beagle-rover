#ifndef _ROBOT_HARDWARE_BEAGLEBONE_MOTORPOWER_H_
#define _ROBOT_HARDWARE_BEAGLEBONE_MOTORPOWER_H_

#include <boost/log/trivial.hpp>
#include "../abstractpower.h"

#include <robotconfig.h>

#if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE

namespace Robot::Hardware::Beaglebone {

    class MotorPower : public Robot::Hardware::AbstractPower {
        protected:
            virtual void enable() override;
            virtual void disable() override;
    };

};

#endif
#endif
