#ifndef _ROBOT_LED_ANIMATION_HEADLIGHTS_H_
#define _ROBOT_LED_ANIMATION_HEADLIGHTS_H_

#include <memory>
#include <robotcontext.h>
#include "abstractanimation.h"

namespace Robot::LED {

    class Headlights : public AbstractAnimation<Headlights> {
        public:
            Headlights(const std::shared_ptr<Robot::Context> &context);

            void init(const std::shared_ptr<ColorLayer> &layer) override;
            void cleanup() override;
    };

}

#endif