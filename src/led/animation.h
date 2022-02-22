#ifndef _ROBOT_LED_ANIMATION_H_
#define _ROBOT_LED_ANIMATION_H_

#include <memory>
#include "types.h"

namespace Robot::LED {

    class Animation {
        public:
            Animation() {}
            virtual ~Animation() = default;

            virtual void init(const std::shared_ptr<Control> &control) = 0;
            virtual void cleanup() = 0;
        
    };

}

#endif
