#include "headlights.h"


namespace Robot::LED {

Headlights::Headlights(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context }
{

}


void Headlights::init()
{
    ColorLayer &layer = *m_layer;

    // Front
    const Color front = Color::WHITE;
    layer[0] = front;
    //layer[1] = front;
    //layer[6] = front;
    layer[7] = front;

    // Rear
    const Color rear = Color(0x40, 0x00, 0x00);
    layer[8] = rear;
    //layer[9] = rear;
    //layer[14] = rear;
    layer[15] = rear;

    layer.setVisible(true);
}


void Headlights::cleanup()
{
    m_layer->setVisible(false);
}



};
