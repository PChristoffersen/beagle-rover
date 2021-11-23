#include "ambulance.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std;
namespace Robot::LED {


static constexpr auto TIMER_INTERVAL { 500ms };
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
        layer[0] = LED_COLOR;
        layer[1] = Color::TRANSPARENT;
        layer[6] = Color::TRANSPARENT;
        layer[7] = LED_COLOR;

        layer[8]  = LED_COLOR;
        layer[9]  = Color::TRANSPARENT;
        layer[14] = Color::TRANSPARENT;
        layer[15] = LED_COLOR;
    }
    else {
        layer[0] = Color::TRANSPARENT;
        layer[1] = LED_COLOR;
        layer[6] = LED_COLOR;
        layer[7] = Color::TRANSPARENT;

        layer[8]  = Color::TRANSPARENT;
        layer[9]  = LED_COLOR;
        layer[14] = LED_COLOR;
        layer[15] = Color::TRANSPARENT;
    }

    layer.show();

    m_state = !m_state;
}


};
