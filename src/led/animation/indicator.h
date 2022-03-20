#ifndef _ROBOT_LED_ANIMATION_INDICATOR_H_
#define _ROBOT_LED_ANIMATION_INDICATOR_H_

#include <memory>
#include <boost/asio.hpp>

#include <robotcontext.h>
#include "abstractanimation.h"
#include "../colorlayer.h"

namespace Robot::LED {

    class Indicator : public AbstractAnimation<Indicator> {
        public:
            Indicator(const strand_type &strand);

            void init(const std::shared_ptr<Control> &control) override;

            void none();
            void left();
            void right();
            void hazard();

        private:
            bool m_state;
            bool m_timer_active;

            void update() override;

            void startTimer();
            void stopTimer();
    };

}

#endif