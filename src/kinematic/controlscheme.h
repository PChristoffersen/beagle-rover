#ifndef _ROBOT_KINEMATIC_CONTROLSCHEME_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEME_H_

#include "types.h"

namespace Robot::Kinematic {

    class ControlScheme {
        public:
            virtual ~ControlScheme() {}

            virtual void init() = 0;
            virtual void cleanup() = 0;

            virtual void updateOrientation(Orientation orientation) = 0;

            virtual void steer(float steering, float throttle, float aux_x, float aux_y) = 0;
    };

};

#endif
