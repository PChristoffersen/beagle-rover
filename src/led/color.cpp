#include "color.h"

#include <cmath>
#include <algorithm>
#include <boost/log/trivial.hpp>



namespace Robot::LED {


Color Color::operator*(brightness_type brightness) const
{
    raw_type scale = std::clamp<raw_type>(brightness*static_cast<brightness_type>(CHANNEL_MAX)+0.5f, CHANNEL_MIN, CHANNEL_MAX);
    if (scale == CHANNEL_MAX) {
        return *this;
    }
    if (scale == CHANNEL_MIN) {
        return Color { m_data & ALPHA_MASK };
    }

    raw_type red   = ( (m_data & RED_MASK)   * scale / CHANNEL_MAX ) & RED_MASK;
    raw_type green = ( (m_data & GREEN_MASK) * scale / CHANNEL_MAX ) & GREEN_MASK;
    raw_type blue  = ( (m_data & BLUE_MASK)  * scale / CHANNEL_MAX ) & BLUE_MASK;
    raw_type alpha = m_data & ALPHA_MASK;

    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" R %+08x (%+02x)-> %+08x") % (m_data & Color::RED_MASK) % static_cast<uint32_t>(brightness) % (red);
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" G %+08x (%+02x)-> %+08x") % (m_data & Color::GREEN_MASK) % static_cast<uint32_t>(brightness) % (green);
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" B %+08x (%+02x)-> %+08x") % (m_data & Color::BLUE_MASK) % static_cast<uint32_t>(brightness) % (blue);
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" A %+08x (%+02x)-> %+08x") % (m_data & Color::ALPHA_MASK) % static_cast<uint32_t>(brightness) % (alpha);

    return Color { alpha | red | green | blue };
}


Color &Color::operator*=(brightness_type brightness)
{
    *this = *this * brightness;
    return *this;
}



Color Color::operator*(const Correction correction) const
{
    raw_type red   = ( (m_data & RED_MASK)   * rawRed(static_cast<raw_type>(correction))   / CHANNEL_MAX ) & RED_MASK;
    raw_type green = ( (m_data & GREEN_MASK) * rawGreen(static_cast<raw_type>(correction)) / CHANNEL_MAX ) & GREEN_MASK;
    raw_type blue  = ( (m_data & BLUE_MASK)  * rawBlue(static_cast<raw_type>(correction))  / CHANNEL_MAX ) & BLUE_MASK;
    raw_type alpha = m_data & ALPHA_MASK;

    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" R %+08x -> %+08x") % (m_data & Color::RED_MASK) % (red);
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" G %+08x -> %+08x") % (m_data & Color::GREEN_MASK) % (green);
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" B %+08x -> %+08x") % (m_data & Color::BLUE_MASK) % (blue);
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format(" A %+08x -> %+08x") % (m_data & Color::ALPHA_MASK) % (alpha);

    return Color { alpha | red | green | blue };
}



Color &Color::operator*=(const Correction correction) 
{
    *this = *this * correction;
    return *this;
}


Color &Color::operator<<(const Color &src_color) {
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " " << src_color;
    Color::raw_type src_a = src_color.alpha();
    auto src = src_color.raw();

    if (src_a==CHANNEL_MIN) {
        return *this;
    }
    if (src_a==CHANNEL_MAX) {
        m_data = src;
        return *this;
    }


    Color::raw_type red   = ( (m_data & Color::RED_MASK)   * (CHANNEL_MAX-src_a+1) / CHANNEL_MAX  +  (src & Color::RED_MASK)   * src_a / CHANNEL_MAX ) & Color::RED_MASK;
    Color::raw_type green = ( (m_data & Color::GREEN_MASK) * (CHANNEL_MAX-src_a+1) / CHANNEL_MAX  +  (src & Color::GREEN_MASK) * src_a / CHANNEL_MAX ) & Color::GREEN_MASK;
    Color::raw_type blue  = ( (m_data & Color::BLUE_MASK)  * (CHANNEL_MAX-src_a+1) / CHANNEL_MAX  +  (src & Color::BLUE_MASK)  * src_a / CHANNEL_MAX ) & Color::BLUE_MASK;
    Color::raw_type alpha = CHANNEL_MAX << Color::ALPHA_SHIFT;

    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format("RedD  %+08x -> %+08x") % (uint32_t)(dst & Color::RED_MASK) % ((dst & Color::RED_MASK)*(0xFF-src_a)/0xFF);
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format("RedS  %+08x -> %+08x") % (uint32_t)(src & Color::RED_MASK) % ((src & Color::RED_MASK)*src_a/0xFF);
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format("  %+08x + %+08x = %+08x   red=%+08x green=%+08x blue=%+08x") % dst % (uint32_t)src % (alpha | red | green | blue) % red % green % blue;

    m_data = alpha | red | green | blue;

    return *this;
}


Color Color::fromHSV(uint16_t hue, uint8_t sat, uint8_t val)
{
    float h = std::clamp<uint16_t>(hue, 0, 360);
    float s = std::clamp<uint8_t>(sat, 0, 100)/100.0f;
    float v = std::clamp<uint8_t>(val, 0, 100)/100.0f;

    float r=0; 
    float g=0; 
    float b=0;

    int i = std::clamp<uint16_t>(hue, 0, 360)/60;
    double f = h / 60.0 - i;
    double pv = v * (1 - s);
    double qv = v * (1 - s*f);
    double tv = v * (1 - s * (1 - f));

    switch (i)
    {
    case 0: //rojo dominante
        r = v;
        g = tv;
        b = pv;
        break;
    case 1: //verde
        r = qv;
        g = v;
        b = pv;
        break;
    case 2: 
        r = pv;
        g = v;
        b = tv;
        break;
    case 3: //azul
        r = pv;
        g = qv;
        b = v;
        break;
    case 4:
        r = tv;
        g = pv;
        b = v;
        break;
    case 5: //rojo
        r = v;
        g = pv;
        b = qv;
        break;
    }

    //set each component to a integer value between 0 and 255
    uint8_t red = std::clamp(r*255.0f, 0.0f, 255.0f);
    uint8_t green = std::clamp(g*255.0f, 0.0f, 255.0f);
    uint8_t blue = std::clamp(b*255.0f, 0.0f, 255.0f);

    //BOOST_LOG_TRIVIAL(info) << "R : " << red;
    //BOOST_LOG_TRIVIAL(info) << "G : " << green;
    //BOOST_LOG_TRIVIAL(info) << "B : " << blue;
    return Color { red, green, blue };
}


}
