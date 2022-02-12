#ifndef _ROBOT_LED_ANIMATION_RAINBOW_H_
#define _ROBOT_LED_ANIMATION_RAINBOW_H_

#include <memory>

#include <robotcontext.h>
#include "abstractanimation.h"

namespace Robot::LED {

    class Rainbow : public AbstractAnimation<Rainbow> {
        public:
            Rainbow(const std::shared_ptr<Robot::Context> &context);

            void init();
            void cleanup();

        private:

            void update(ColorLayer &layer);
    };

}

#endif