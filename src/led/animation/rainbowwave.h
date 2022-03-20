#ifndef _ROBOT_LED_ANIMATION_RAINBOWWAVE_H_
#define _ROBOT_LED_ANIMATION_RAINBOWWAVE_H_

#include <cstdint>
#include <memory>

#include <robotcontext.h>
#include "abstractanimation.h"
#include "../colorlayer.h"

namespace Robot::LED {

    class RainbowWave : public AbstractAnimation<RainbowWave> {
        public:
            RainbowWave(const strand_type &strand);

            void init(const std::shared_ptr<Control> &control) override;

        private:
            std::uint8_t m_hue;

            void update() override;
    };

}

#endif