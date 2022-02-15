#ifndef _ROBOT_LED_ANIMATION_AMBULANCE_H_
#define _ROBOT_LED_ANIMATION_AMBULANCE_H_

#include <memory>
#include <boost/asio.hpp>

#include <robotcontext.h>
#include "abstractanimation.h"

namespace Robot::LED {

    class Ambulance : public AbstractAnimation<Ambulance> {
        public:
            Ambulance(const std::shared_ptr<Robot::Context> &context);

            void init(const std::shared_ptr<ColorLayer> &layer) override;
            void cleanup() override;

        private:
            std::shared_ptr<ColorLayer> m_layer;
            bool m_state;

            void update() override;
    };

}

#endif