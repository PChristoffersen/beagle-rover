#ifndef _ROBOT_LED_ANIMATION_CONSTRUCTION_H_
#define _ROBOT_LED_ANIMATION_CONSTRUCTION_H_

#include <memory>
#include <boost/asio.hpp>

#include <robotcontext.h>
#include "abstractanimation.h"
#include "../colorlayer.h"

namespace Robot::LED {

    class Construction : public AbstractAnimation<Construction> {
        public:
            Construction(const strand_type &strand);

            void init(const std::shared_ptr<Control> &control) override;

        private:
            bool m_state;

            void update() override;
    };

}

#endif