#include <iostream>
#include <boost/exception/all.hpp>
#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

#include <robotcontrol-ext.h>

#include "ledcontrol.h"
#include "../robotcontext.h"

using namespace boost;
using namespace boost::asio;


#define RGB_PIXEL(r,g,b) ((uint32_t)(r<<16|g<<8|b))

LEDControl::LEDControl(shared_ptr<RobotContext> context) 
{

}

void LEDControl::init() {
    setAll(LED_BLACK);
}


void LEDControl::cleanup() {
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


void LEDControl::updatePixels(LEDColorList const &pixels) {
    uint32_t pix[LED_PIXEL_COUNT];
    int i=0;
    for (auto iter = pixels.begin(); iter != pixels.end(); ++iter) {
        pix[i] = pixels[i];
        BOOST_LOG_TRIVIAL(info) << i << "  " << format("%+08x") % pix[i];
        i++;
        if (i>=LED_PIXEL_COUNT) 
            break;
    }
    while (i<LED_PIXEL_COUNT) {
        pix[i] = LED_BLACK;
        BOOST_LOG_TRIVIAL(info) << i << "  BLACK";
        i++;
    }
    _updatePixels(pix);
}


void LEDControl::setAll(const LEDColor &color) {
    uint32_t pixels[LED_PIXEL_COUNT];
    for (int i=0; i<LED_PIXEL_COUNT; i++) {
        pixels[i] = color;
    }
    _updatePixels(pixels);
}


void LEDControl::_updatePixels(uint32_t pixels[LED_PIXEL_COUNT]) {
    if (rc_ext_neopixel_set(pixels, LED_PIXEL_COUNT)!=0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error updating pixels"));
    }
}
