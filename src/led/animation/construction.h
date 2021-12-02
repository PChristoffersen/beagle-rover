#ifndef _ROBOT_LED_ANIMATION_CONSTRUCTION_H_
#define _ROBOT_LED_ANIMATION_CONSTRUCTION_H_

#include <memory>
#include <boost/asio.hpp>

#include <robotcontext.h>
#include "abstractanimation.h"

namespace Robot::LED {

    class Construction : public AbstractAnimation<Construction> {
        public:
            Construction(const std::shared_ptr<Robot::Context> &context);

            void init();
            void cleanup();

        private:
            bool m_state;

            void update(ColorLayer &layer);
    };

};

#endif