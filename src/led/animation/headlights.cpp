#include "headlights.h"


namespace Robot::LED {

static constexpr Color FRONT_COLOR { Color::WHITE };
static constexpr Color REAR_COLOR { 0x40, 0x00, 0x00 };


Headlights::Headlights(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context }
{

}


void Headlights::init(const std::shared_ptr<ColorLayer> &layer)
{
    auto &front { layer->segments()[0] };
    auto &back { layer->segments()[1] };

    layer->fill(Color::TRANSPARENT);

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

    layer->setVisible(true);
    layer->show();
}


void Headlights::cleanup()
{
}



}
