#include "control.h"

#include <algorithm>
#include <thread>
#include <boost/exception/all.hpp>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

#include <robotcontext.h>
#include "animation/headlights.h"
#include "animation/indicator.h"
#include "animation/construction.h"
#include "animation/police.h"
#include "animation/ambulance.h"
#include "animation/knightrider.h"
#include "animation/rainbow.h"

using namespace std::literals;

namespace Robot::LED {

static constexpr auto LED_UPDATE_INTERVAL { 30ms };


Control::Control(const std::shared_ptr<Robot::Context> &context) :
    m_context { context },
    m_initialized { false },
    m_background { Color::BLACK },
    m_animation_mode { AnimationMode::NONE },
    m_indicator_mode { IndicatorMode::NONE }
{

}


Control::~Control() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void Control::init() 
{
    const guard lock(m_mutex);

    m_initialized = true;
    clear(Color::BLACK);

    m_update_signal = std::make_shared<::Robot::ASyncSignal>(m_context->io(), LED_UPDATE_INTERVAL);
    m_update_connection = m_update_signal->connect([self_ptr=this->weak_from_this()](auto cnt) {
        if (auto self = self_ptr.lock()) { 
            self->updatePixels(); 
        }
    });
    m_update_signal->async_wait();

    m_indicator_layer = std::make_shared<ColorLayer>("Indicators", LAYER_DEPTH_INDICATORS, true);
    m_animation_layer = std::make_shared<ColorLayer>("Animation", LAYER_DEPTH_ANIMATION, true);
    
    attachLayer(m_indicator_layer);
    attachLayer(m_animation_layer);

    m_indicator = std::make_shared<Indicator>(m_context);
    m_indicator->init(m_indicator_layer);

}


void Control::cleanup() 
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;

    m_update_connection.disconnect();
    m_update_signal->cancel();
    m_update_signal = nullptr;

    if (m_animation) {
        m_animation->cleanup();
        m_animation = nullptr;
    }
    if (m_indicator) {
        m_indicator->cleanup();
        m_indicator = nullptr;
    }

    for (auto &l : m_layers) {
        l->clearSignal();
    }
    m_layers.clear();
    m_indicator_layer = nullptr;
    m_animation_layer = nullptr;

    std::this_thread::sleep_for(50us);
    clear(Color::BLACK);
}


#if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
void Control::setLED(rc_led_t led, bool state)
{
    const guard lock(m_mutex);
    rc_led_set(led, state?1:0);
}
#endif


void Control::clear(const Color &color) 
{
    const guard lock(m_mutex);

    m_pixels.fill(color);
    showPixels();
}


void Control::showPixels()
{
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    if (rc_ext_neopixel_set(pixels.data())!=0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error updating pixels"));
    }
    #endif
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_PC
    std::stringstream sstream;
    for (auto &pixel : m_pixels) {
        sstream << boost::format("%+02x%+02x%+02x  ") % (uint32_t)Color::rawRed(pixel) % (uint32_t)Color::rawGreen(pixel) % (uint32_t)Color::rawBlue(pixel);
    }
    BOOST_LOG_TRIVIAL(info) << *this << " " << sstream.str();
    #endif
}


void Control::setBackground(const Color &color) 
{
    const guard lock(m_mutex);
    if (m_background!=color) {
        m_background = color;
        update();
        notify(NOTIFY_DEFAULT);
    }
}



void Control::setAnimation(AnimationMode mode) 
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(info) << "Animation: " << (int)mode;
    if (mode!=m_animation_mode) {
        if (m_animation) {
            m_animation->cleanup();
            m_animation = nullptr;
        }

        switch (mode) {
        case AnimationMode::NONE:
            break;
        case AnimationMode::HEADLIGHTS:
            m_animation = std::make_shared<Headlights>(m_context);
            break;
        case AnimationMode::CONSTRUCTION:
            m_animation = std::make_shared<Construction>(m_context);
            break;
        case AnimationMode::POLICE:
            m_animation = std::make_shared<Police>(m_context);
            break;
        case AnimationMode::AMBULANCE:
            m_animation = std::make_shared<Ambulance>(m_context);
            break;
        case AnimationMode::KNIGHT_RIDER:
            m_animation = std::make_shared<KnightRider>(m_context);
            break;
        case AnimationMode::RAINBOW:
            m_animation = std::make_shared<Rainbow>(m_context);
            break;
        }


        m_animation_mode = mode;

        if (m_animation) {
            m_animation->init(m_animation_layer);
        }
        else {
            m_animation_layer->fill(Color::TRANSPARENT);
            m_animation_layer->setVisible(false);
            update();
        }

        notify(NOTIFY_DEFAULT);
    }
}


void Control::setIndicators(IndicatorMode mode) 
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(info) << "Indicator: " << (int)mode;
    if (mode!=m_indicator_mode) {
        switch (mode) {
        case IndicatorMode::NONE:
            m_indicator->none();
            break;
        case IndicatorMode::LEFT:
            m_indicator->left();
            break;
        case IndicatorMode::RIGHT:
            m_indicator->right();
            break;
        case IndicatorMode::HAZARD:
            m_indicator->hazard();
            break;
        }
        m_indicator_mode = mode;
        notify(NOTIFY_DEFAULT);
    }
}


void Control::update()
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    if (m_update_signal) {
        (*m_update_signal)();
    }
}


Control::LayerList Control::layers()
{
    const guard lock(m_mutex);
    return m_layers;
}


RawColorArray Control::pixels()
{
    const guard lock(m_mutex);
    return m_pixels;
}


void Control::updatePixels()
{
    BOOST_LOG_TRIVIAL(trace) << "Show >> ";
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    
    BOOST_LOG_TRIVIAL(trace) << "Control::Show()";

    m_pixels.fill(m_background);

    for (const auto &layer : m_layers) {
        m_pixels << *layer;
    }

    showPixels();
    notify(NOTIFY_UPDATE);

    BOOST_LOG_TRIVIAL(trace) << "Show << ";
}


void Control::attachLayer(const std::shared_ptr<ColorLayer> &layer)
{
    BOOST_LOG_TRIVIAL(trace) << "Attach Layer >> " << *layer;
    const guard lock(m_mutex);

    for (auto &l : m_layers) {
        if (l==layer) {
            BOOST_LOG_TRIVIAL(warning) << "Layer " << layer->depth() << " already attached";
            return;
        }
    }

    bool inserted = false;
    for (auto ith=m_layers.begin(); ith!=m_layers.end(); ++ith) {
        const auto &l = *ith;
        if (l->depth() > layer->depth()) {
            m_layers.insert(ith, layer);
            inserted = true;
            break;
        }
    }
    if (!inserted) {
        m_layers.push_back(layer);
    }
    layer->setSignal(m_update_signal);
    if (layer->visible()) {
        update();
    }
    notify(NOTIFY_DEFAULT);
    BOOST_LOG_TRIVIAL(trace) << "Attach Layer << " << *layer;
}


void Control::detachLayer(const std::shared_ptr<ColorLayer> &layer) 
{
    BOOST_LOG_TRIVIAL(trace) << "Detach Layer >> " << *layer;
    const guard lock(m_mutex);
    bool updated = false;
    m_layers.remove_if([layer, &updated](const auto &l) {
        if (l==layer) {
            updated |= true;
            layer->clearSignal();
            return true;
        }
        return false;
    });
    if (updated) {
        update();
        notify(NOTIFY_DEFAULT);
    }
    BOOST_LOG_TRIVIAL(info) << "Detach Layer << " << *layer;
}


}