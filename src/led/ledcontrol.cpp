#include <iostream>
#include <boost/exception/all.hpp>
#include <boost/bind.hpp>

#include <robotcontrol-ext.h>

#include "ledcontrol.h"

using namespace boost;
using namespace boost::asio;


#define RGB_PIXEL(r,g,b) ((uint32_t)(r<<16|g<<8|b))

LEDControl::LEDControl(shared_ptr<io_context> io) :
    Component(io)
{

}

void LEDControl::init() {
    setAll(0x00, 0x00, 0x04);
    Component::init();
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
    setAll(0x00, 0x00, 0x00);
    Component::cleanup();    
}



void LEDControl::setAll(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t pixels[LED_PIXEL_COUNT];
    uint32_t color = RGB_PIXEL(r,g,b);
    for (int i=0; i<LED_PIXEL_COUNT; i++) {
        pixels[i] = color;
    }
    updatePixels(pixels);
}


void LEDControl::updatePixels(uint32_t pixels[LED_PIXEL_COUNT]) {
    if (rc_ext_neopixel_set(pixels, LED_PIXEL_COUNT)!=0) {
        BOOST_THROW_EXCEPTION(std::runtime_error("Error updating pixels"));
    }
}
