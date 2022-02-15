#ifndef _ROBOT_LED_ANIMATION_POLICE_H_
#define _ROBOT_LED_ANIMATION_POLICE_H_

#include <memory>

#include <robotcontext.h>
#include "abstractanimation.h"

namespace Robot::LED {

    class Police : public AbstractAnimation<Police> {
        public:
            Police(const std::shared_ptr<Robot::Context> &context);

            void init(const std::shared_ptr<ColorLayer> &layer) override;
            void cleanup() override;

        private:
            bool m_state;
            std::shared_ptr<ColorLayer> m_layer;

            void update() override;
    };

}

#endif