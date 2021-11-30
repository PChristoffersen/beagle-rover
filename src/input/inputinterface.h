#ifndef _INPUT_INPUTINTERFACE_H_
#define _INPUT_INPUTINTERFACE_H_

#include "inputvalue.h"

namespace Robot::Input {

    class InputInterface {
        public:
            virtual void steer(double steer, double throttle, double aux_x=0.0, double aux_y=0.0) = 0;
    };

};


#endif
