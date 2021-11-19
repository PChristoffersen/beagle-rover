#include "ledcolor.h"

#include <boost/format.hpp>
#include <boost/log/trivial.hpp>


using namespace std;
using namespace Robot::LED;



const Color Color::TRANSPARENT { 0x00, 0x00, 0x00, 0x00 };
const Color Color::BLACK       { 0x00, 0x00, 0x00 };
const Color Color::WHITE       { 0xFF, 0xFF, 0xFF };
const Color Color::RED         { 0xFF, 0x00, 0x00 };
const Color Color::GREEN       { 0x00, 0xFF, 0x00 };
const Color Color::BLUE        { 0x00, 0x00, 0xFF };



inline Color::raw_type Color::raw_argb(raw_type a, raw_type r, raw_type g, raw_type b) {
    return a<<ALPHA_SHIFT | r<<RED_SHIFT | g<<GREEN_SHIFT | b<<BLUE_SHIFT;
}


Color::Color() : 
    Color(TRANSPARENT)
{
}

Color::Color(raw_type data) :
    m_data { data } 
{
}

Color::Color(uint8_t r, uint8_t g, uint8_t b) :
    m_data { raw_argb(0xFF, r, g, b) }
{
}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint32_t a) :
    m_data { raw_argb(a, r, g, b) }
{
}


Color::Color(const Color &color) :
    m_data{ color.m_data }
{
}


Color &Color::operator=(const Color &other) 
{
    m_data = other.m_data;
    return *this;    
}


Color &Color::operator=(const raw_type other) {
    m_data = other;
    return *this;    
}


Color Color::operator+(const Color &other) const {
    return Color { m_data + other };
}


Color::raw_type Robot::LED::operator+(Color::raw_type dst, const Color &src_color) {
    Color::raw_type src_a = src_color.alpha();
    if (src_a==0x00) {
        return dst;
    }

    Color::raw_type src = src_color.data();

    //res.r = dst.r * (1 - src.a) + src.r * src.a
    //res.g = dst.g * (1 - src.a) + src.g * src.a
    //res.b = dst.b * (1 - src.a) + src.b * src.a
    //res.a = dst.a * (1 - src.a) + src.a

    Color::raw_type red   = (dst & Color::RED_MASK)*(0xFF-src_a)/0xFF   + (src & Color::RED_MASK)*src_a/0xFF;
    Color::raw_type green = (dst & Color::GREEN_MASK)*(0xFF-src_a)/0xFF + (src & Color::GREEN_MASK)*src_a/0xFF;
    Color::raw_type blue  = (dst & Color::BLUE_MASK)*(0xFF-src_a)/0xFF  + (src & Color::BLUE_MASK)*src_a/0xFF;
    Color::raw_type alpha = 0xFF << Color::ALPHA_SHIFT;

    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format("  %+08x + %+08x = %+08x   red=%+08x green=%+08x blue=%+08x") % dst % (uint32_t)src % (alpha | red | green | blue) % red % green % blue;

    return alpha | red | green | blue;
}


