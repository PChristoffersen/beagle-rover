#ifndef _ROBOT_HARDWARE_NOOPPOWER_H_
#define _ROBOT_HARDWARE_NOOPPOWER_H_

#include <boost/log/trivial.hpp>
#include "abstractpower.h"

namespace Robot::Hardware {

    class NoopPower : public AbstractPower {
        protected:
            virtual void enable() override 
            {
            }
            virtual void disable() override
            {
            }
    };

};

#endif
