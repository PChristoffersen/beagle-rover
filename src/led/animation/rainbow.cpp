#include "rainbow.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std::literals;

namespace Robot::LED {


static constexpr auto TIMER_INTERVAL { 100ms };


Rainbow::Rainbow(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, TIMER_INTERVAL }
{
}


void Rainbow::init(const std::shared_ptr<ColorLayer> &layer)
{
    m_layer = layer;
    m_timer.expires_after(TIMER_INTERVAL);
    timerSetup();
    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(true);
}


void Rainbow::cleanup()
{
    m_timer.cancel();
    m_layer->setVisible(false);
    m_layer = nullptr;
}


void Rainbow::update() 
{
    const ColorLayer::guard lock(m_layer->mutex());
    m_layer->fill(Color::TRANSPARENT);
    m_layer->show();
}


}
