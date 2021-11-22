#ifndef _LED_ANIMATION_H_
#define _LED_ANIMATION_H_

#include <memory>
#include "ledcolorlayer.h"

namespace Robot::LED {

    class Animation {
        public:
            Animation() : m_layer { std::make_shared<ColorLayer>(LAYER_DEPTH_ANIMATION) } {}
            Animation(int depth) : m_layer { std::make_shared<ColorLayer>(depth) } {}
            virtual ~Animation() = default;

            virtual void init() = 0;
            virtual void cleanup() = 0;
        
            const std::shared_ptr<ColorLayer> &layer() { return m_layer; }

        protected:
            std::shared_ptr<ColorLayer> m_layer;
    };

};

#endif
