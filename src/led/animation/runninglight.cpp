#include "runninglight.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std::literals;

namespace Robot::LED {


static constexpr auto LAYER_NAME { "runninglight" };
static constexpr auto TIMER_INTERVAL { 50ms };


RunningLight::RunningLight(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, TIMER_INTERVAL },
    m_pos { 0 }
{
    m_layer = std::make_shared<ColorLayer>(LAYER_NAME, LAYER_DEPTH_ANIMATION, true);
}


void RunningLight::init(const std::shared_ptr<Control> &control)
{
    AbstractAnimation::init(control);
    
    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(true);

    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
}




void RunningLight::update() 
{
    const ColorLayer::guard lock(m_layer->mutex());
    
    ColorLayer &layer = *m_layer;
    Color color { Color::HSV { m_hue, 0xFF, 0xFF }};
    
    layer.fill(Color::TRANSPARENT);

    layer[m_pos] = color.withAlpha(0.5f);
    layer[(m_pos+1)%layer.size()] = color;
    
    m_pos++;
    m_hue++;

    if (m_pos>layer.size()) {
        m_pos = 0;
    }

    layer.update();
}


}
