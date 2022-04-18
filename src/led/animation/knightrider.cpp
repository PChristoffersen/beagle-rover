#include "knightrider.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std::literals;

namespace Robot::LED {


static constexpr auto LAYER_NAME { "knightrider" };
static constexpr auto TIMER_INTERVAL { 100ms };
static constexpr Color LED_COLOR1 { 0x40, 0x00, 0x00, 0xFF };
static constexpr Color LED_COLOR2 { 0x10, 0x00, 0x00, 0x80 };


KnightRider::KnightRider(const strand_type &strand) :
    AbstractAnimation { strand, TIMER_INTERVAL },
    m_pos { 0 },
    m_dir { 1 }
{
    m_layer = std::make_shared<ColorLayer>(LAYER_NAME, LAYER_DEPTH_ANIMATION, true);
}


void KnightRider::init(const std::shared_ptr<Control> &control)
{
    AbstractAnimation::init(control);

    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(true);

    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
}




void KnightRider::update() 
{
    const ColorLayer::guard lock(m_layer->mutex());
    auto &front { m_layer->segment(FRONT_SEGMENT) };
    
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

    m_layer->update();

}


}
