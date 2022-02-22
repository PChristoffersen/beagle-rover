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
#include "animation/runninglight.h"
#include "animation/knightrider.h"
#include "animation/rainbow.h"
#include "animation/rainbowwave.h"

using namespace std::literals;

namespace Robot::LED {

static constexpr auto LED_UPDATE_INTERVAL { 30ms };


Control::Control(const std::shared_ptr<Robot::Context> &context) :
    m_context { context },
    m_initialized { false },
    m_brightness { Color::BRIGHTNESS_DEFAULT },
    m_color_correction { Color::Correction::TypicalLEDStrip },
    m_background { Color::BLACK },
    m_pixels { DEFAULT_SEGMENTS },
    m_animation_mode { AnimationMode::NONE },
    m_indicator_mode { IndicatorMode::NONE }
{

}


Control::~Control() 
{
    cleanup();
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

    m_indicator = std::make_shared<Indicator>(m_context);
    m_indicator->init(shared_from_this());

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

    for (auto &l : m_layers) {
        l->clearSignal();
    }
    m_layers.clear();

    if (m_animation) {
        m_animation->cleanup();
        m_animation = nullptr;
    }
    if (m_indicator) {
        m_indicator->cleanup();
        m_indicator = nullptr;
    }


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
    // Apply color correction and brightness
    color_array_type pixels { m_pixels };
    pixels *= m_color_correction;

    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    if (rc_ext_neopixel_set(RawColorArray(pixels).data())!=0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error updating pixels"));
    }
    #endif
    #if ROBOT_PLATFORM == ROBOT_PLATFORM_PC
    std::stringstream sstream;
    #if 0
    for (auto &color : pixels) {
        sstream << boost::format("%+02x%+02x%+02x  ") % (uint32_t)color.red() % (uint32_t)color.green() % (uint32_t)color.blue();
    }
    #else
    for (auto &color : m_pixels) {
        sstream << boost::format("%+02x%+02x%+02x  ") % (uint32_t)color.red() % (uint32_t)color.green() % (uint32_t)color.blue();
    }
    #endif
    BOOST_LOG_TRIVIAL(info) << *this << " (" << pixels.size() << ") " << sstream.str();
    #endif
}


void Control::setBrightness(Color::brightness_type brightness)
{
    const guard lock(m_mutex);
    brightness = Color::clampBrightness(brightness);
    if (m_brightness!=brightness) {
        m_brightness = brightness;
        if (m_initialized) {
            update();
            notify(NOTIFY_DEFAULT);
        }
    }
}


void Control::setColorCorrection(Color::Correction correction)
{
    const guard lock(m_mutex);
    if (m_color_correction!=correction) {
        m_color_correction = correction;
        if (m_initialized) {
            update();
            notify(NOTIFY_DEFAULT);
        }
    }
}


void Control::setBackground(Color color) 
{
    const guard lock(m_mutex);
    color = color.opaque();
    if (m_background!=color) {
        m_background = color;
        if (m_initialized) {
            update();
            notify(NOTIFY_DEFAULT);
        }
    }
}



void Control::setAnimation(AnimationMode mode) 
{
    const guard lock(m_mutex);
    //BOOST_LOG_TRIVIAL(trace) << "Animation: " << (int)mode;
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
        case AnimationMode::RUNNING_LIGHT:
            m_animation = std::make_shared<RunningLight>(m_context);
            break;
        case AnimationMode::KNIGHT_RIDER:
            m_animation = std::make_shared<KnightRider>(m_context);
            break;
        case AnimationMode::RAINBOW:
            m_animation = std::make_shared<Rainbow>(m_context);
            break;
        case AnimationMode::RAINBOW_WAVE:
            m_animation = std::make_shared<RainbowWave>(m_context);
            break;
        }


        m_animation_mode = mode;

        if (m_animation) {
            m_animation->init(shared_from_this());
        }
        notify(NOTIFY_DEFAULT);
    }
}


void Control::setIndicators(IndicatorMode mode) 
{
    const guard lock(m_mutex);
    //BOOST_LOG_TRIVIAL(trace) << "Indicator: " << (int)mode;
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


Control::LayerList Control::layers(bool filter_internal) const
{
    const guard lock(m_mutex);
    if (filter_internal) {
        LayerList res;
        std::copy_if(m_layers.begin(), m_layers.end(), std::back_inserter(res), [](auto &l) { return !l->internal(); });
        return res;
    }
    return m_layers;
}


void Control::updatePixels()
{
    const guard lock(m_mutex);
    if (!m_initialized)
        return;
    
    //BOOST_LOG_TRIVIAL(trace) << "Control::Show()";

    {
        const color_array_type::guard pix_lock(m_pixels.mutex());
        m_pixels.fill(m_background);
        for (const auto &layer : m_layers) {
            m_pixels << *layer;
        }
        m_pixels *= m_brightness;
    }

    showPixels();
    notify(NOTIFY_UPDATE);
}


void Control::attachLayer(const std::shared_ptr<ColorLayer> &layer)
{
    BOOST_LOG_TRIVIAL(trace) << "Attach Layer " << *layer;
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
}


void Control::detachLayer(const std::shared_ptr<ColorLayer> &layer) 
{
    const guard lock(m_mutex);
    bool updated = false;
    m_layers.remove_if([layer, &updated](const auto &l) {
        if (l==layer) {
            BOOST_LOG_TRIVIAL(trace) << "Detach Layer " << *layer;
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
}


}