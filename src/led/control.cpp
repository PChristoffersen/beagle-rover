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

static constexpr auto LED_UPDATE_INTERVAL { 60ms };


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

    m_show_signal = std::make_shared<::Robot::ASyncSignal>(m_context->io(), LED_UPDATE_INTERVAL);
    m_show_connection = m_show_signal->connect([self_ptr=this->weak_from_this()](auto cnt) {
        if (auto self = self_ptr.lock()) { 
            self->updatePixels(); 
        }
    });
    m_show_signal->async_wait();

    m_indicator_layer = std::make_shared<ColorLayer>(LAYER_DEPTH_INDICATORS);
    m_animation_layer = std::make_shared<ColorLayer>(LAYER_DEPTH_ANIMATION);
    
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

    m_show_connection.disconnect();
    m_show_signal->cancel();
    m_show_signal = nullptr;

    if (m_animation) {
        m_animation->cleanup();
        m_animation = nullptr;
    }
    if (m_indicator) {
        m_indicator->cleanup();
        m_indicator = nullptr;
    }

    for (auto &ptr : m_layers) {
        if (const auto &l = ptr.lock()) {
            l->clearSignal();
        }
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

    RawColorArray pixels;
    pixels.fill(color);

    showPixels(pixels);
}


void Control::showPixels(const RawColorArray &pixels)
{
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
            show();
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


void Control::show()
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    if (m_show_signal) {
        (*m_show_signal)();
    }
}

void Control::updatePixels()
{
    BOOST_LOG_TRIVIAL(trace) << "Show >> ";
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    
    BOOST_LOG_TRIVIAL(trace) << "Control::Show()";

    RawColorArray pixels;
    pixels.fill(m_background);

    for (const auto &ptr : m_layers) {
        if (const auto &layer = ptr.lock()) {
            pixels << *layer;
        }
    }

    showPixels(pixels);

    BOOST_LOG_TRIVIAL(trace) << "Show << ";
}


void Control::attachLayer(const std::shared_ptr<ColorLayer> &layer)
{
    BOOST_LOG_TRIVIAL(trace) << "Attach Layer >> " << *layer;
    const guard lock(m_mutex);

    for (auto &ptr : m_layers) {
        if (auto l = ptr.lock()) {
            if (l==layer) {
                BOOST_LOG_TRIVIAL(warning) << "Layer " << layer->depth() << " already attached";
                return;
            }
        }
    }

    bool inserted = false;
    for (auto ith=m_layers.begin(); ith!=m_layers.end(); ++ith) {
        const auto &ptr = *ith;
        const auto &l = ptr.lock();
        if (!l) {
            continue;
        }
        if (l->depth() > layer->depth()) {
            m_layers.insert(ith, layer);
            inserted = true;
            break;
        }
    }
    if (!inserted) {
        m_layers.push_back(layer);
    }
    layer->setSignal(m_show_signal);
    if (layer->visible()) {
        show();
    }
    notify(NOTIFY_DEFAULT);
    BOOST_LOG_TRIVIAL(info) << "Attach Layer << " << *layer;
}


void Control::detachLayer(const std::shared_ptr<ColorLayer> &layer) 
{
    BOOST_LOG_TRIVIAL(trace) << "Detach Layer >> " << *layer;
    const guard lock(m_mutex);
    bool updated = false;
    m_layers.remove_if([layer, &updated](const auto &ptr) {
        if (const auto &l = ptr.lock()) {
            if (l==layer) {
                updated |= true;
                layer->clearSignal();
                return true;
            }
        }
        else {
            // Stale layer
            updated = true;
            return true;
        }
        return false;
    });
    if (updated) {
        show();
        notify(NOTIFY_DEFAULT);
    }
    BOOST_LOG_TRIVIAL(info) << "Detach Layer << " << *layer;
}


}