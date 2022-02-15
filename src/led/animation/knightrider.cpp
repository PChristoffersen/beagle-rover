#include "knightrider.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std::literals;

namespace Robot::LED {


static constexpr auto TIMER_INTERVAL { 100ms };
static constexpr Color LED_COLOR1 { 0x40, 0x00, 0x00, 0xFF };
static constexpr Color LED_COLOR2 { 0x40, 0x00, 0x00, 0x40 };


KnightRider::KnightRider(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, TIMER_INTERVAL },
    m_pos { 0 },
    m_dir { 1 }
{

}


void KnightRider::init(const std::shared_ptr<ColorLayer> &layer)
{
    m_layer = layer;
    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(true);
}


void KnightRider::cleanup()
{
    m_timer.cancel();
    m_layer->setVisible(false);
    m_layer = nullptr;
}


void KnightRider::update() 
{
    const ColorLayer::guard lock(m_layer->mutex());
    auto &front { m_layer->segments()[0] };
    
    m_layer->fill(Color::TRANSPARENT);

    if (m_pos<=0) {
        front[m_pos] = LED_COLOR1;
        m_dir = 1;
    }
    else if (m_pos>=front.size()-1) {
        front[m_pos] = LED_COLOR1;
        m_dir = -1;
    }
    else {
        front[m_pos] = LED_COLOR1;
        front[m_pos-m_dir] = LED_COLOR2;
    }

    m_pos += m_dir;

    m_layer->show();

}


}
