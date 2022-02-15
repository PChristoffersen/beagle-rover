#ifndef _ROBOT_LED_ANIMATION_INDICATOR_H_
#define _ROBOT_LED_ANIMATION_INDICATOR_H_

#include <memory>
#include <boost/asio.hpp>

#include <robotcontext.h>
#include "abstractanimation.h"

namespace Robot::LED {

    class Indicator : public AbstractAnimation<Indicator> {
        public:
            Indicator(const std::shared_ptr<Robot::Context> &context);

            void init(const std::shared_ptr<ColorLayer> &layer) override;
            void cleanup() override;

            void none();
            void left();
            void right();
            void hazard();

        private:
            bool m_state;
            bool m_timer_active;
            std::shared_ptr<ColorLayer> m_layer;

            void update() override;

            void startTimer();
            void stopTimer();
    };

}

#endif