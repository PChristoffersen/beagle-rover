#include "ambulance.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std;
namespace Robot::LED {


static constexpr auto TIMER_INTERVAL { 250ms };
static constexpr Color LED_COLOR { 0x00, 0x00, 0xFF };


Ambulance::Ambulance(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, TIMER_INTERVAL },
    m_state { false }
{

}


void Ambulance::init()
{
    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
    m_layer->setVisible(true);
}


void Ambulance::cleanup()
{
    m_timer.cancel();
    m_layer->setVisible(false);
}


void Ambulance::update(ColorLayer &layer) 
{
    Color color { 0x00, 0xFF, 0xFF };
    if (m_state) {
        layer.front[0] = LED_COLOR;
        layer.front[1] = Color::TRANSPARENT;
        layer.front[6] = Color::TRANSPARENT;
        layer.front[7] = LED_COLOR;

        layer.back[0] = LED_COLOR;
        layer.back[1] = Color::TRANSPARENT;
        layer.back[6] = Color::TRANSPARENT;
        layer.back[7] = LED_COLOR;
    }
    else {
        layer.front[0] = Color::TRANSPARENT;
        layer.front[1] = LED_COLOR;
        layer.front[6] = LED_COLOR;
        layer.front[7] = Color::TRANSPARENT;

        layer.back[0]  = Color::TRANSPARENT;
        layer.back[1]  = LED_COLOR;
        layer.back[6] = LED_COLOR;
        layer.back[7] = Color::TRANSPARENT;
    }

    layer.show();

    m_state = !m_state;
}


};
