#include "construction.h"

#include <chrono>
#include <boost/log/trivial.hpp>

using namespace std::literals;

namespace Robot::LED {


static constexpr auto LAYER_NAME { "construction" };
static constexpr auto TIMER_INTERVAL { 250ms };
static constexpr Color LED_COLOR { 0xEE, 0xBE, 0x00 };

Construction::Construction(const strand_type &strand) :
    AbstractAnimation { strand, TIMER_INTERVAL },
    m_state { false }
{
    m_layer = std::make_shared<ColorLayer>(LAYER_NAME, LAYER_DEPTH_ANIMATION, true);
}


void Construction::init(const std::shared_ptr<Control> &control)
{
    AbstractAnimation::init(control);

    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(true);

    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
}


void Construction::update() 
{
    const ColorLayer::guard lock(m_layer->mutex());
    auto &front { m_layer->segments()[0] };
    auto &back { m_layer->segments()[1] };

    if (m_state) {
        front[0] = LED_COLOR;
        front[1] = Color::TRANSPARENT;
        front[6] = Color::TRANSPARENT;
        front[7] = LED_COLOR;

        back[0]  = LED_COLOR;
        back[1]  = Color::TRANSPARENT;
        back[6] = Color::TRANSPARENT;
        back[7] = LED_COLOR;
    }
    else {
        front[0] = Color::TRANSPARENT;
        front[1] = LED_COLOR;
        front[6] = LED_COLOR;
        front[7] = Color::TRANSPARENT;

        back[0]  = Color::TRANSPARENT;
        back[1]  = LED_COLOR;
        back[6] = LED_COLOR;
        back[7] = Color::TRANSPARENT;
    }

    m_layer->update();

    m_state = !m_state;
}


}
