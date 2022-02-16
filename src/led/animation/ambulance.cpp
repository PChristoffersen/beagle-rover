#include "ambulance.h"

#include <chrono>
#include <boost/log/trivial.hpp>

using namespace std::literals;

namespace Robot::LED {


static constexpr auto TIMER_INTERVAL { 250ms };
static constexpr Color LED_COLOR { 0x00, 0x00, 0xFF };


Ambulance::Ambulance(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, TIMER_INTERVAL },
    m_state { false }
{

}


void Ambulance::init(const std::shared_ptr<ColorLayer> &layer)
{
    m_layer = layer;
    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(true);
    m_layer->update();
}


void Ambulance::cleanup()
{
    m_timer.cancel();
    m_layer->setVisible(false);
    m_layer = nullptr;
}


void Ambulance::update() 
{
    const ColorLayer::guard lock(m_layer->mutex());
    auto &front { m_layer->segments()[0] };
    auto &back { m_layer->segments()[1] };

    if (m_state) {
        front[0] = LED_COLOR;
        front[1] = Color::TRANSPARENT;
        front[6] = Color::TRANSPARENT;
        front[7] = LED_COLOR;

        back[0] = LED_COLOR;
        back[1] = Color::TRANSPARENT;
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
