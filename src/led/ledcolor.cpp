#include "ledcolor.h"

#define RGB_PIXEL(r,g,b) ((uint32_t)(r<<16|g<<8|b))


LEDColor::LEDColor(uint8_t r, uint8_t g, uint8_t b) :
    m_red(r),
    m_green(g),
    m_blue(b)
{
}


LEDColor::LEDColor(const LEDColor &color) :
    m_red(color.m_red),
    m_green(color.m_green),
    m_blue(color.m_blue)
{
}

LEDColor &LEDColor::operator=(const LEDColor &other) {
    m_red = other.m_red;
    m_green = other.m_green;
    m_blue = other.m_blue;
    return *this;    
}


LEDColor::operator uint32_t() const {
    return RGB_PIXEL(m_red, m_green, m_blue);
}


LEDColor LED_BLACK(0x00, 0x00, 0x00);
