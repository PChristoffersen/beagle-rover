#include "police.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std::literals;

namespace Robot::LED {


static constexpr auto TIMER_INTERVAL { 300ms };
static constexpr Color LED_COLOR1 { 0xFF, 0x00, 0x00 };
static constexpr Color LED_COLOR2 { 0x00, 0x00, 0xFF };


Police::Police(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, TIMER_INTERVAL },
    m_state { false }
{

}


void Police::init()
{
    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
    m_layer->setVisible(true);
}


void Police::cleanup()
{
    m_timer.cancel();
    m_layer->setVisible(false);
}


void Police::update(ColorLayer &layer) 
{
    auto &front { layer.segments()[0] };
    auto &back { layer.segments()[1] };

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

    layer.show();

    m_state = !m_state;
}


};
