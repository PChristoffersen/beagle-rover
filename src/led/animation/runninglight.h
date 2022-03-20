#ifndef _ROBOT_LED_ANIMATION_RUNNINGLIGHT_H_
#define _ROBOT_LED_ANIMATION_RUNNINGLIGHT_H_

#include <memory>

#include <robotcontext.h>
#include "abstractanimation.h"
#include "../colorlayer.h"

namespace Robot::LED {

    class RunningLight : public AbstractAnimation<RunningLight> {
        public:
            RunningLight(const strand_type &strand);

            void init(const std::shared_ptr<Control> &control) override;

        private:
            std::uint8_t m_hue;
            uint m_pos;

            void update() override;
    };

}

#endif