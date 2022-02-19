#include "rainbow.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std::literals;

namespace Robot::LED {


static constexpr auto TIMER_INTERVAL { 80ms };


Rainbow::Rainbow(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, TIMER_INTERVAL },
    m_hue { 0 }
{
}


void Rainbow::init(const std::shared_ptr<ColorLayer> &layer)
{
    m_layer = layer;
    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(true);
    m_layer->update();
}


void Rainbow::cleanup()
{
    m_timer.cancel();
    m_layer->setVisible(false);
    m_layer = nullptr;
}


void Rainbow::update() 
{
    const ColorLayer::guard lock(m_layer->mutex());
    auto &layer { *m_layer };

    for (auto i=0u; i<m_layer->size(); i++) {
        layer[i] = Color::HSV { static_cast<std::uint8_t>(m_hue + i * 8), 255, 255 };
    }
    m_hue += 8;

    m_layer->update();
}


}
