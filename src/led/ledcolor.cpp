#include "ledcolor.h"

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>


using namespace std;




const LEDColor LEDColor::TRANSPARENT { 0x00, 0x00, 0x00, 0x00 };
const LEDColor LEDColor::BLACK       { 0x00, 0x00, 0x00 };
const LEDColor LEDColor::WHITE       { 0xFF, 0xFF, 0xFF };
const LEDColor LEDColor::RED         { 0xFF, 0x00, 0x00 };
const LEDColor LEDColor::GREEN       { 0x00, 0xFF, 0x00 };
const LEDColor LEDColor::BLUE        { 0x00, 0x00, 0xFF };



inline LEDColor::raw_type LEDColor::raw_argb(raw_type a, raw_type r, raw_type g, raw_type b) {
    return a<<ALPHA_SHIFT | r<<RED_SHIFT | g<<GREEN_SHIFT | b<<BLUE_SHIFT;
}


LEDColor::LEDColor() : 
    LEDColor(TRANSPARENT)
{
}

LEDColor::LEDColor(raw_type data) :
    m_data { data } 
{
}

LEDColor::LEDColor(uint8_t r, uint8_t g, uint8_t b) :
    m_data { raw_argb(0xFF, r, g, b) }
{
}

LEDColor::LEDColor(uint8_t r, uint8_t g, uint8_t b, uint32_t a) :
    m_data { raw_argb(a, r, g, b) }
{
}


LEDColor::LEDColor(const LEDColor &color) :
    m_data{ color.m_data }
{
}


LEDColor &LEDColor::operator=(const LEDColor &other) 
{
    m_data = other.m_data;
    return *this;    
}


LEDColor &LEDColor::operator=(const raw_type other) {
    m_data = other;
    return *this;    
}


LEDColor LEDColor::operator+(const LEDColor &other) const {
    return LEDColor { m_data + other };
}


LEDColor::raw_type operator+(LEDColor::raw_type dst, const LEDColor &src_color) {
    LEDColor::raw_type src_a = src_color.alpha();
    if (src_a==0x00) {
        return dst;
    }

    LEDColor::raw_type src = src_color.data();

    //res.r = dst.r * (1 - src.a) + src.r * src.a
    //res.g = dst.g * (1 - src.a) + src.g * src.a
    //res.b = dst.b * (1 - src.a) + src.b * src.a
    //res.a = dst.a * (1 - src.a) + src.a

    LEDColor::raw_type red   = (dst & LEDColor::RED_MASK)*(0xFF-src_a)/0xFF   + (src & LEDColor::RED_MASK)*src_a/0xFF;
    LEDColor::raw_type green = (dst & LEDColor::GREEN_MASK)*(0xFF-src_a)/0xFF + (src & LEDColor::GREEN_MASK)*src_a/0xFF;
    LEDColor::raw_type blue  = (dst & LEDColor::BLUE_MASK)*(0xFF-src_a)/0xFF  + (src & LEDColor::BLUE_MASK)*src_a/0xFF;
    LEDColor::raw_type alpha = 0xFF << LEDColor::ALPHA_SHIFT;

    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format("  %+08x + %+08x = %+08x   red=%+08x green=%+08x blue=%+08x") % dst % (uint32_t)src % (alpha | red | green | blue) % red % green % blue;

    return alpha | red | green | blue;
}


