#ifndef _ROBOT_LED_ANIMATION_POLICE_H_
#define _ROBOT_LED_ANIMATION_POLICE_H_

#include <memory>

#include <robotcontext.h>
#include "abstractanimation.h"

namespace Robot::LED {

    class Police : public AbstractAnimation<Police> {
        public:
            Police(const std::shared_ptr<Robot::Context> &context);

            void init();
            void cleanup();

        private:
            bool m_state;

            void update(ColorLayer &layer);
    };

};

#endif