#include "knightrider.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std;
namespace Robot::LED {


static constexpr auto TIMER_INTERVAL { 100ms };
static constexpr Color LED_COLOR1 { 0xFF, 0x00, 0x00, 0x40 };
static constexpr Color LED_COLOR2 { 0xFF, 0x00, 0x00, 0x08 };


KnightRider::KnightRider(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, TIMER_INTERVAL },
    m_pos { 0 },
    m_dir { 1 }
{

}


void KnightRider::init()
{
    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
    m_layer->setVisible(true);
}


void KnightRider::cleanup()
{
    m_timer.cancel();
    m_layer->setVisible(false);
}


void KnightRider::update(ColorLayer &layer) 
{
    layer.fill(Color::TRANSPARENT);

    if (m_pos==0) {
        layer[m_pos] = LED_COLOR1;
        m_dir = 1;
    }
    else if (m_pos==7) {
        layer[m_pos] = LED_COLOR1;
        m_dir = -1;
    }
    else {
        layer[m_pos] = LED_COLOR1;
        layer[m_pos-m_dir] = LED_COLOR2;
    }

    m_pos += m_dir;

    layer.show();

}


};
