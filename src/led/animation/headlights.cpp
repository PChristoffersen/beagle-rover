#include "headlights.h"


namespace Robot::LED {

static constexpr auto LAYER_NAME { "headlights" };
static constexpr Color FRONT_COLOR { Color::WHITE };
static constexpr Color REAR_COLOR { Color::RED * 0.5f };


Headlights::Headlights(const strand_type &strand) :
    AbstractAnimation { strand }
{
    m_layer = std::make_shared<ColorLayer>(LAYER_NAME, LAYER_DEPTH_ANIMATION, true);
}


void Headlights::init(const std::shared_ptr<Control> &control)
{
    AbstractAnimation::init(control);

    m_layer->fill(Color::TRANSPARENT);
    
    auto &front { m_layer->segments()[0] };
    auto &back { m_layer->segments()[1] };


    // Front
    front[0] = FRONT_COLOR;
    //front[1] = FRONT_COLOR;
    //front[6] = FRONT_COLOR;
    front[7] = FRONT_COLOR;

    // Rear
    back[0] = REAR_COLOR;
    //back[1] = REAR_COLOR;
    //back[6] = REAR_COLOR;
    back[7] = REAR_COLOR;

    m_layer->setVisible(true);
    m_layer->update();
}


}
