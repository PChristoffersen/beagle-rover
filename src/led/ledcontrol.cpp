#include <iostream>
#include <boost/exception/all.hpp>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

#include <robotcontrolext.h>

#include "ledcontrol.h"
#include "../robotcontext.h"


using namespace std;


const int LEDControl::LED_PIXEL_COUNT = RC_EXT_NEOPIXEL_COUNT;


LEDControl::LEDControl(shared_ptr<RobotContext> context) :
    m_initialized { false }
{

}


LEDControl::~LEDControl() 
{
    cleanup();
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void LEDControl::init() 
{
    setAll(LED_BLACK);
    m_initialized = true;
}


void LEDControl::cleanup() 
{
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
    setAll(LED_BLACK);
}


void LEDControl::updatePixels(LEDColorList const &pixels) 
{
    uint32_t pix[LED_PIXEL_COUNT];
    int i=0;
    for (auto &p : pixels) {
        pix[i] = p;
        BOOST_LOG_TRIVIAL(info) << i << "  " << boost::format("%+08x") % pix[i];
        i++;
        if (i>=LED_PIXEL_COUNT) 
            break;
    }
    while (i<LED_PIXEL_COUNT) {
        pix[i] = LED_BLACK;
        i++;
    }
    _updatePixels(pix);
}


void LEDControl::setAll(const LEDColor &color) 
{
    uint32_t pixels[LED_PIXEL_COUNT];
    for (int i=0; i<LED_PIXEL_COUNT; i++) {
        pixels[i] = color;
    }
    _updatePixels(pixels);
}


void LEDControl::_updatePixels(uint32_t pixels[LED_PIXEL_COUNT]) 
{
    if (rc_ext_neopixel_set(pixels, LED_PIXEL_COUNT)!=0) {
        BOOST_THROW_EXCEPTION(runtime_error("Error updating pixels"));
    }
}
