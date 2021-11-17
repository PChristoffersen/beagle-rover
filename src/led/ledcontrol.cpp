#include "ledcontrol.h"

#include <algorithm>
#include <boost/exception/all.hpp>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

#include "../robotcontext.h"


using namespace std;


LEDControl::LEDControl(shared_ptr<RobotContext> context) :
    m_initialized { false },
    m_background { LEDColor::BLACK }
{

}


LEDControl::~LEDControl() 
{
    cleanup();
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void LEDControl::init() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_initialized = true;
    clear(LEDColor::BLACK);
}


void LEDControl::cleanup() 
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
    clear(LEDColor::BLACK);
}


void LEDControl::clear(const LEDColor &color) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);

    LEDRawColorArray pixels;
    pixels.fill(LEDColor::BLACK);

    if (rc_ext_neopixel_set(pixels.data(), pixels.size())!=0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error updating pixels"));
    }
}


void LEDControl::setBackground(const LEDColor &color) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_background = color;
}


void LEDControl::show() 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    
    BOOST_LOG_TRIVIAL(trace) << "LEDControl::Show()";
    
    LEDRawColorArray pixels;
    pixels.fill(m_background);
    for (const auto &layer : m_layers) {
        pixels += *layer;
    }

    #if 0
    for (int i=0; i<pixels.size(); ++i) {
        BOOST_LOG_TRIVIAL(info) << boost::format("    out %2d %+08x") % i % pixels[i];
    }
    #endif

    if (rc_ext_neopixel_set(pixels.data(), pixels.size())!=0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error updating pixels"));
    }
}


void LEDControl::addLayer(std::shared_ptr<class LEDColorLayer> &layer)
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    for (auto ith=m_layers.begin(); ith!=m_layers.end(); ++ith) {
        const auto &elm = *ith;
        if (elm->depth()>layer->depth()) {
            m_layers.insert(ith, layer);
            return;
        }
    }
    m_layers.push_back(layer);
}


void LEDControl::removeLayer(std::shared_ptr<class LEDColorLayer> &layer) 
{
    const lock_guard<recursive_mutex> lock(m_mutex);
    m_layers.remove(layer);
}
