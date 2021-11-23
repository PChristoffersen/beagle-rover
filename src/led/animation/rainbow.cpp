#include "rainbow.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std;
namespace Robot::LED {


static constexpr auto TIMER_INTERVAL { 100ms };


Rainbow::Rainbow(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, TIMER_INTERVAL }
{
}


void Rainbow::init()
{
    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
    m_layer->setVisible(true);
}


void Rainbow::cleanup()
{
    m_timer.cancel();
    m_layer->setVisible(false);
}


void Rainbow::update(ColorLayer &layer) 
{
    layer.fill(Color::TRANSPARENT);
    layer.show();
}


};
