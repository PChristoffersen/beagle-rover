#ifndef _ROBOT_INPUT_SOFTWAREINTERFACE_H_
#define _ROBOT_INPUT_SOFTWAREINTERFACE_H_

#include "value.h"

namespace Robot::Input {

    class SoftwareInterface {
        public:
            virtual void steer(double steer, double throttle, double aux_x=0.0, double aux_y=0.0) = 0;
    };

};


#endif
