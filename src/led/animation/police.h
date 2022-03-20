#ifndef _ROBOT_LED_ANIMATION_POLICE_H_
#define _ROBOT_LED_ANIMATION_POLICE_H_

#include <memory>

#include <robotcontext.h>
#include "abstractanimation.h"
#include "../colorlayer.h"

namespace Robot::LED {

    class Police : public AbstractAnimation<Police> {
        public:
            Police(const strand_type &strand);

            void init(const std::shared_ptr<Control> &control) override;

        private:
            bool m_state;

            void update() override;
    };

}

#endif