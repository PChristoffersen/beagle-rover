#ifndef _ROBOT_LED_ANIMATION_KNIGHTRIDER_H_
#define _ROBOT_LED_ANIMATION_KNIGHTRIDER_H_

#include <memory>

#include <robotcontext.h>
#include "abstractanimation.h"

namespace Robot::LED {

    class KnightRider : public AbstractAnimation<KnightRider> {
        public:
            KnightRider(const std::shared_ptr<Robot::Context> &context);

            void init();
            void cleanup();

        private:
            int m_pos;
            int m_dir;

            void update(ColorLayer &layer);
    };

}

#endif