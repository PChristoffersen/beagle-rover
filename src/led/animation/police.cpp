#include "police.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std::literals;

namespace Robot::LED {


static constexpr auto LAYER_NAME { "police" };
static constexpr auto TIMER_INTERVAL { 300ms };
static constexpr Color LED_COLOR1 { 0xFF, 0x00, 0x00 };
static constexpr Color LED_COLOR2 { 0x00, 0x00, 0xFF };


Police::Police(const strand_type &strand) :
    AbstractAnimation { strand, TIMER_INTERVAL },
    m_state { false }
{
    m_layer = std::make_shared<ColorLayer>(LAYER_NAME, LAYER_DEPTH_ANIMATION, true);
}


void Police::init(const std::shared_ptr<Control> &control)
{
    AbstractAnimation::init(control);

    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(true);

    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
}



void Police::update() 
{
    const ColorLayer::guard lock(m_layer->mutex());
    auto &front { m_layer->segment(FRONT_SEGMENT) };
    auto &back { m_layer->segment(BACK_SEGMENT) };

    if (m_state) {
        front[0] = LED_COLOR1;
        front[1] = LED_COLOR2;
        front[6] = LED_COLOR1;
        front[7] = LED_COLOR2;

        back[0] = LED_COLOR1;
        back[1] = LED_COLOR2;
        back[6] = LED_COLOR1;
        back[7] = LED_COLOR2;
    }
    else {
        front[0] = LED_COLOR2;
        front[1] = LED_COLOR1;
        front[6] = LED_COLOR2;
        front[7] = LED_COLOR1;

        back[0] = LED_COLOR2;
        back[1] = LED_COLOR1;
        back[6] = LED_COLOR2;
        back[7] = LED_COLOR1;
    }

    m_layer->update();

    m_state = !m_state;
}


}
