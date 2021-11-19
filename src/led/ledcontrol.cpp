#include "ledcontrol.h"

#include <algorithm>
#include <thread>
#include <boost/exception/all.hpp>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

#include "../robotcontext.h"


using namespace std;

namespace Robot::LED {


Control::Control(shared_ptr<Robot::Context> context) :
    m_initialized { false },
    m_background { Color::BLACK }
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
}


void Control::cleanup() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (!m_initialized) 
        return;
    m_initialized = false;
    #if 0
    while (!m_animation.empty()) {
        LEDAnimation *animation = m_animation.top();
        animation->stop();
        m_animation.pop();
        delete animation;
    }
    #endif
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
        pixels += *layer;
    }

    #if 0
    for (int i=0; i<pixels.size(); ++i) {
        BOOST_LOG_TRIVIAL(info) << boost::format("    out %2d %+08x") % i % pixels[i];
    }
    #endif

    if (rc_ext_neopixel_set(pixels.data())!=0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error updating pixels"));
    }

    m_last_show = chrono::high_resolution_clock::now();
}


void Control::attachLayer(std::shared_ptr<ColorLayer> &layer)
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


void Control::detachLayer(std::shared_ptr<ColorLayer> &layer) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    if (auto self = layer->control().lock()) {
        if (self==shared_from_this()) {
            layer->detach();
            m_layers.remove(layer);
        }
    }
}

void Control::removeLayer(std::shared_ptr<ColorLayer> &layer) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_layers.remove(layer);
}

};