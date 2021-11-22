#ifndef _ANIMATION_INDICATOR_H_
#define _ANIMATION_INDICATOR_H_

#include <memory>
#include <boost/asio.hpp>

#include "../../robotcontext.h"
#include "abstractanimation.h"

namespace Robot::LED {

    class Indicator : public AbstractAnimation<Indicator> {
        public:
            Indicator(const std::shared_ptr<Robot::Context> &context);

            void init();
            void cleanup();

            void none();
            void left();
            void right();
            void hazard();

        private:
            bool m_state;
            bool m_timer_active;

            void update(ColorLayer &layer);

            void startTimer();
            void stopTimer();
    };

};

#endif