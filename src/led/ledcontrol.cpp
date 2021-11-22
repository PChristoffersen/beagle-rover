#include "ledcontrol.h"

#include <algorithm>
#include <thread>
#include <boost/exception/all.hpp>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

#include "../robotcontext.h"
#include "animation/headlights.h"
#include "animation/indicator.h"
#include "animation/construction.h"
#include "animation/police.h"
#include "animation/ambulance.h"
#include "animation/knightrider.h"
#include "animation/rainbow.h"


using namespace std;

namespace Robot::LED {


Control::Control(const shared_ptr<Robot::Context> &context) :
    m_context { context },
    m_initialized { false },
    m_background { Color::BLACK },
    m_animation_mode { AnimationMode::NONE }
{

}


Control::~Control() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void Control::init() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_initialized = true;
    clear(Color::BLACK);

    m_indicator = make_shared<Indicator>(m_context);
    m_indicator->init();
    attachLayer(m_indicator->layer());
}


void Control::cleanup() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
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


void Control::clear(const Color &color) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    RawColorArray pixels;
    pixels.fill(color);

    if (rc_ext_neopixel_set(pixels.data())!=0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error updating pixels"));
    }
}


void Control::setBackground(const Color &color) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_background = color;
}



void Control::setAnimation(AnimationMode mode) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
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
            m_animation = make_shared<Headlights>(m_context);
            break;
        case AnimationMode::CONSTRUCTION:
            m_animation = make_shared<Construction>(m_context);
            break;
        case AnimationMode::POLICE:
            m_animation = make_shared<Police>(m_context);
            break;
        case AnimationMode::AMBULANCE:
            m_animation = make_shared<Ambulance>(m_context);
            break;
        case AnimationMode::KNIGHT_RIDER:
            m_animation = make_shared<KnightRider>(m_context);
            break;
        case AnimationMode::RAINBOW:
            m_animation = make_shared<Rainbow>(m_context);
            break;
        }


        m_animation_mode = mode;

        if (m_animation) {
            m_animation->init();
            attachLayer(m_animation->layer());
        }
        show();
    }
}


void Control::setIndicators(IndicatorMode mode) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
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
    }
}


void Control::show()
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    
    BOOST_LOG_TRIVIAL(trace) << "Control::Show()";

    // If interval between show() is too low insert a very short delay
    if ( (chrono::high_resolution_clock::now()-m_last_show) < 50us ) {
        this_thread::sleep_for(50us);
    }

    RawColorArray pixels;
    pixels.fill(m_background);
    for (const auto &layer : m_layers) {
        pixels << *layer;
    }

    #if 0
    for (int i=0; i<pixels.size(); ++i) {
        BOOST_LOG_TRIVIAL(info) << boost::format("    out %2d ") % i << Color(pixels[i]);
    }
    #endif

    if (rc_ext_neopixel_set(pixels.data())!=0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error updating pixels"));
    }

    m_last_show = chrono::high_resolution_clock::now();
}


void Control::attachLayer(const std::shared_ptr<ColorLayer> &layer)
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << " depth=" << layer->depth();

    auto self = shared_from_this();

    if (auto other = layer->control().lock()) {
        if (other==self) {
            BOOST_LOG_TRIVIAL(warning) << "Layer " << layer->depth() << " already attached";
            return;
        }
        BOOST_THROW_EXCEPTION(invalid_argument("Layer already attached"));
    }

    layer->setControl(self);

    for (auto ith=m_layers.begin(); ith!=m_layers.end(); ++ith) {
        const auto &elm = *ith;
        if (elm->depth()>layer->depth()) {
            m_layers.insert(ith, layer);
            return;
        }
    }
    m_layers.push_back(layer);
}


void Control::detachLayer(const std::shared_ptr<ColorLayer> &layer) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (auto self = layer->control().lock()) {
        if (self==shared_from_this()) {
            layer->detach();
            m_layers.remove(layer);
        }
    }
}

void Control::removeLayer(const std::shared_ptr<ColorLayer> &layer) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_layers.remove(layer);
}

};