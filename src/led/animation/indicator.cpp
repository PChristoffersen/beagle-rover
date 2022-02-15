#include "indicator.h"

#include <chrono>
#include <boost/log/trivial.hpp>

using namespace std::literals;

namespace Robot::LED {


static constexpr auto TIMER_INTERVAL { 500ms };
static constexpr Color INDICATOR_COLOR { 0xEE, 0xBE, 0x00 };

Indicator::Indicator(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, TIMER_INTERVAL },
    m_state { false },
    m_timer_active { false }
{
}


void Indicator::init(const std::shared_ptr<ColorLayer> &layer)
{
    m_layer = layer;
    m_layer->setVisible(false);
    m_layer->fill(Color::TRANSPARENT);
}


void Indicator::cleanup()
{
    m_timer.cancel();
    m_timer_active = false;
    m_layer->setVisible(false);
    m_layer = nullptr;
}


void Indicator::none()
{
    const ColorLayer::guard lock { m_layer->mutex() };
    m_timer.cancel();
    m_timer_active = false;
    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(false);
    m_layer->show();
    stopTimer();
}

void Indicator::left()
{
    const ColorLayer::guard lock { m_layer->mutex() };
    auto &front { m_layer->segments()[0] };
    auto &back { m_layer->segments()[1] };

    m_layer->fill(Color::TRANSPARENT);
    front[6] = INDICATOR_COLOR;
    front[7] = INDICATOR_COLOR;
    back[0] = INDICATOR_COLOR;
    back[1] = INDICATOR_COLOR;
    startTimer();
}

void Indicator::right() 
{
    const ColorLayer::guard lock { m_layer->mutex() };
    auto &front { m_layer->segments()[0] };
    auto &back { m_layer->segments()[1] };

    m_layer->fill(Color::TRANSPARENT);
    front[0] = INDICATOR_COLOR;
    front[1] = INDICATOR_COLOR;
    back[6] = INDICATOR_COLOR;
    back[7] = INDICATOR_COLOR;
    startTimer();
}

void Indicator::hazard()
{
    const ColorLayer::guard lock { m_layer->mutex() };
    auto &front { m_layer->segments()[0] };
    auto &back { m_layer->segments()[1] };

    front[0] = INDICATOR_COLOR;
    front[1] = INDICATOR_COLOR;
    front[6] = INDICATOR_COLOR;
    front[7] = INDICATOR_COLOR;
    back[0] = INDICATOR_COLOR;
    back[1] = INDICATOR_COLOR;
    back[6] = INDICATOR_COLOR;
    back[7] = INDICATOR_COLOR;
    startTimer();
}


void Indicator::startTimer() 
{
    if (!m_timer_active) {
        m_timer_active = true;
        m_timer.expires_after(TIMER_INTERVAL);
        timerSetup();
    }
}

void Indicator::stopTimer()
{
    if (m_timer_active) {
        m_timer.cancel();
        m_timer_active = false;
    }
}

void Indicator::update() 
{
    m_state = !m_state;
    m_layer->setVisible(m_state);
    m_layer->show();
}


}
