#include "ledcolor.h"


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
    return (uint32_t)m_red<<16|(uint32_t)m_green<<8|(uint32_t)m_blue;
}


LEDColor LED_BLACK(0x00, 0x00, 0x00);
