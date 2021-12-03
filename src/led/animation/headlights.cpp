#include "headlights.h"


namespace Robot::LED {

static constexpr Color FRONT_COLOR { Color::WHITE };
static constexpr Color REAR_COLOR { 0x40, 0x00, 0x00 };


Headlights::Headlights(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context }
{

}


void Headlights::init()
{
    ColorLayer &layer = *m_layer;

    // Front
    layer.front[0] = FRONT_COLOR;
    //layer.front[1] = FRONT_COLOR;
    //layer.front[6] = FRONT_COLOR;
    layer.front[7] = FRONT_COLOR;

    // Rear
    layer.back[0] = REAR_COLOR;
    //layer.back[1] = REAR_COLOR;
    //layer.back[6] = REAR_COLOR;
    layer.back[7] = REAR_COLOR;

    layer.setVisible(true);
    layer.show();
}


void Headlights::cleanup()
{
    m_layer->setVisible(false);
}



};
