#include "rainbow.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std::literals;

namespace Robot::LED {


static constexpr auto LAYER_NAME { "rainbow" };
static constexpr auto TIMER_INTERVAL { 20ms };



Rainbow::Rainbow(const strand_type &strand) :
    AbstractAnimation { strand, TIMER_INTERVAL },
    m_hue { 0 }
{
    m_layer = std::make_shared<ColorLayer>(LAYER_NAME, LAYER_DEPTH_ANIMATION, true);
}


void Rainbow::init(const std::shared_ptr<Control> &control)
{
    AbstractAnimation::init(control);

    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(true);

    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
}


void Rainbow::update() 
{
    const ColorLayer::guard lock(m_layer->mutex());
    auto &layer { *m_layer };

    Color color { Color::HSV { m_hue, 255, 255 } };
    for (auto i=0u; i<m_layer->size(); i++) {
        layer[i] = color;
    }
    m_hue++;

    m_layer->update();
}


}
