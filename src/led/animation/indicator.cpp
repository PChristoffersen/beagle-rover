#include "indicator.h"

#include <chrono>
#include <boost/log/trivial.hpp>


using namespace std;
namespace Robot::LED {


static constexpr auto TIMER_INTERVAL { 500ms };
static constexpr Color INDICATOR_COLOR { 0xEE, 0xBE, 0x00 };

Indicator::Indicator(const std::shared_ptr<Robot::Context> &context) :
    AbstractAnimation { context, LAYER_DEPTH_INDICATORS, TIMER_INTERVAL },
    m_state { false },
    m_timer_active { false }
{
}


void Indicator::init()
{
    m_layer->fill(Color::TRANSPARENT);
}


void Indicator::cleanup()
{
    m_timer.cancel();
    m_timer_active = false;
    m_layer->setVisible(false);
}


void Indicator::none()
{
    const ColorLayerLock lock { m_layer };
    m_timer.cancel();
    m_timer_active = false;
    m_layer->fill(Color::TRANSPARENT);
    m_layer->setVisible(false);
    m_layer->show();
    stopTimer();
}

void Indicator::left()
{
    const ColorLayerLock lock { m_layer };
    m_layer->fill(Color::TRANSPARENT);
    (*m_layer)[0] = INDICATOR_COLOR;
    (*m_layer)[1] = INDICATOR_COLOR;
    (*m_layer)[8] = INDICATOR_COLOR;
    (*m_layer)[9] = INDICATOR_COLOR;
    startTimer();
}

void Indicator::right() 
{
    const ColorLayerLock lock { m_layer };
    m_layer->fill(Color::TRANSPARENT);
    (*m_layer)[6] = INDICATOR_COLOR;
    (*m_layer)[7] = INDICATOR_COLOR;
    (*m_layer)[14] = INDICATOR_COLOR;
    (*m_layer)[15] = INDICATOR_COLOR;
    startTimer();
}

void Indicator::hazard()
{
    const ColorLayerLock lock { m_layer };
    (*m_layer)[0] = INDICATOR_COLOR;
    (*m_layer)[1] = INDICATOR_COLOR;
    (*m_layer)[6] = INDICATOR_COLOR;
    (*m_layer)[7] = INDICATOR_COLOR;
    (*m_layer)[8] = INDICATOR_COLOR;
    (*m_layer)[9] = INDICATOR_COLOR;
    (*m_layer)[14] = INDICATOR_COLOR;
    (*m_layer)[15] = INDICATOR_COLOR;
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

void Indicator::update(ColorLayer &layer) 
{
    m_state = !m_state;
    layer.setVisible(m_state);
    layer.show();
}


};
