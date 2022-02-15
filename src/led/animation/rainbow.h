#ifndef _ROBOT_LED_ANIMATION_RAINBOW_H_
#define _ROBOT_LED_ANIMATION_RAINBOW_H_

#include <memory>

#include <robotcontext.h>
#include "abstractanimation.h"

namespace Robot::LED {

    class Rainbow : public AbstractAnimation<Rainbow> {
        public:
            Rainbow(const std::shared_ptr<Robot::Context> &context);

            void init(const std::shared_ptr<ColorLayer> &layer) override;
            void cleanup() override;

        private:
            std::shared_ptr<ColorLayer> m_layer;

            void update() override;
    };

}

#endif