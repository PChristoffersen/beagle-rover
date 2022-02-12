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

    m_indicator = std::make_shared<Indicator>(m_context);
    m_indicator->init();
    attachLayer(m_indicator->layer());
}


void Control::cleanup() 
{
    const guard lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;
    if (m_animation) {
        detachLayer(m_animation->layer());
        m_animation->cleanup();
        m_animation = nullptr;
    }
    if (m_indicator) {
        detachLayer(m_indicator->layer());
        m_indicator->cleanup();
        m_indicator = nullptr;
    }

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

    RawColorArray pixels;
    pixels.fill(color);

    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    if (rc_ext_neopixel_set(pixels.data())!=0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error updating pixels"));
    }
    #endif
}


void Control::setBackground(const Color &color) 
{
    const guard lock(m_mutex);
    if (m_background!=color) {
        m_background = color;
        notify(NOTIFY_DEFAULT);
    }
}



void Control::setAnimation(AnimationMode mode) 
{
    const guard lock(m_mutex);
    BOOST_LOG_TRIVIAL(info) << "Animation: " << (int)mode;
    if (mode!=m_animation_mode) {
        if (m_animation) {
            detachLayer(m_animation->layer());
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
            m_animation->init();
            attachLayer(m_animation->layer());
        }
        show();

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


void Control::show()
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    
    BOOST_LOG_TRIVIAL(trace) << "Control::Show()";

    // If interval between show() is too low insert a very short delay
    if ( (clock_type::now()-m_last_show) < 50us ) {
        std::this_thread::sleep_for(50us);
    }

    RawColorArray pixels;
    pixels.fill(m_background);

    for (const auto &layer_ptr : m_layers) {
        if (const auto &layer = layer_ptr.lock()) {
            pixels << *layer;
        }
    }

    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    if (rc_ext_neopixel_set(pixels.data())!=0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error updating pixels"));
    }
    #endif
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_PC
    std::stringstream sstream;
    for (auto &pixel : pixels) {
        sstream << boost::format("%+02x%+02x%+02x  ") % (uint32_t)Color::rawRed(pixel) % (uint32_t)Color::rawGreen(pixel) % (uint32_t)Color::rawBlue(pixel);
    }
    BOOST_LOG_TRIVIAL(info) << *this << " " << sstream.str();
    #endif

    m_last_show = clock_type::now();
}


void Control::attachLayer(const std::shared_ptr<ColorLayer> &layer)
{
    const guard lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << " depth=" << layer->depth();

    auto self = shared_from_this();

    if (auto other = layer->control().lock()) {
        if (other==self) {
            BOOST_LOG_TRIVIAL(warning) << "Layer " << layer->depth() << " already attached";
            return;
        }
        BOOST_THROW_EXCEPTION(std::invalid_argument("Layer already attached"));
    }

    layer->setControl(self);

    for (auto ith=m_layers.begin(); ith!=m_layers.end(); ++ith) {
        const auto &elm = *ith;
        const auto &l = elm.lock();
        if (!l) {
            continue;
        }
        if (l->depth() > layer->depth()) {
            m_layers.insert(ith, layer->weak_from_this());
            return;
        }
    }
    m_layers.push_back(layer->weak_from_this());
    notify(NOTIFY_DEFAULT);
}


void Control::detachLayer(const std::shared_ptr<ColorLayer> &layer) 
{
    BOOST_LOG_TRIVIAL(trace) << "Detach Layer " << *layer;
    const guard lock(m_mutex);
    if (auto self = layer->control().lock()) {
        if (self==shared_from_this()) {
            layer->detach();
        }
    }
}

void Control::removeLayer(const std::shared_ptr<ColorLayer> &layer) 
{
    const guard lock(m_mutex);
    bool removed = false;
    m_layers.remove_if([layer, &removed](const auto &elm) {
        if (const auto &l = elm.lock()) {
            removed |= (l==layer);
            return l == layer;
        }
        else {
            // Stale layer
            removed = true;
            return true;
        }
    });
    if (removed) {
        show();
        notify(NOTIFY_DEFAULT);
    }
}


}