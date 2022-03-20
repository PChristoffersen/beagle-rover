#ifndef _ROBOT_LED_ANIMATION_HEADLIGHTS_H_
#define _ROBOT_LED_ANIMATION_HEADLIGHTS_H_

#include <memory>
#include <robotcontext.h>
#include "abstractanimation.h"
#include "../colorlayer.h"

namespace Robot::LED {

    class Headlights : public AbstractAnimation<Headlights> {
        public:
            Headlights(const strand_type &strand);

            void init(const std::shared_ptr<Control> &control) override;

    };

}

#endif