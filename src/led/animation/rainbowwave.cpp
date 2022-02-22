#include "rainbowwave.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std::literals;

namespace Robot::LED {


static constexpr auto LAYER_NAME { "rainbowwave" };
static constexpr auto TIMER_INTERVAL { 20ms };


RainbowWave::RainbowWave(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, TIMER_INTERVAL },
    m_hue { 0 }
{
    m_layer = std::make_shared<ColorLayer>(LAYER_NAME, LAYER_DEPTH_ANIMATION, true);
}


void RainbowWave::init(const std::shared_ptr<Control> &control)
{
    AbstractAnimation::init(control);

    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(true);

    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
}


void RainbowWave::update() 
{
    const ColorLayer::guard lock(m_layer->mutex());
    auto &layer { *m_layer };

    for (auto i=0u; i<m_layer->size(); i++) {
        layer[i] = Color::HSV { static_cast<std::uint8_t>(m_hue + i * 8), 255, 255 };
    }
    m_hue++;

    m_layer->update();
}


}
