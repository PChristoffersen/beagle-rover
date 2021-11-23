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
    layer[0] = FRONT_COLOR;
    //layer[1] = FRONT_COLOR;
    //layer[6] = FRONT_COLOR;
    layer[7] = FRONT_COLOR;

    // Rear
    layer[8] = REAR_COLOR;
    //layer[9] = REAR_COLOR;
    //layer[14] = REAR_COLOR;
    layer[15] = REAR_COLOR;

    layer.setVisible(true);
}


void Headlights::cleanup()
{
    m_layer->setVisible(false);
}



};
