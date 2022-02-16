#include "color.h"

#include <cmath>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>



namespace Robot::LED {



Color::raw_type &operator<<(Color::raw_type &dst, const Color &src_color) {
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " " << src_color;
    Color::raw_type src_a = src_color.alpha();
    if (src_a==0x00) {
        return dst;
    }

    Color::raw_type src = src_color.data();

    //res.r = dst.r * (1 - src.a) + src.r * src.a
    //res.g = dst.g * (1 - src.a) + src.g * src.a
    //res.b = dst.b * (1 - src.a) + src.b * src.a
    //res.a = dst.a * (1 - src.a) + src.a

    Color::raw_type red   = ( (dst & Color::RED_MASK)*(0xFF-src_a)/0xFF   + (src & Color::RED_MASK)*src_a/0xFF   ) & Color::RED_MASK;
    Color::raw_type green = ( (dst & Color::GREEN_MASK)*(0xFF-src_a)/0xFF + (src & Color::GREEN_MASK)*src_a/0xFF ) & Color::GREEN_MASK;
    Color::raw_type blue  = ( (dst & Color::BLUE_MASK)*(0xFF-src_a)/0xFF  + (src & Color::BLUE_MASK)*src_a/0xFF  ) & Color::BLUE_MASK;
    Color::raw_type alpha = 0xFF << Color::ALPHA_SHIFT;

    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format("RedD  %+08x -> %+08x") % (uint32_t)(dst & Color::RED_MASK) % ((dst & Color::RED_MASK)*(0xFF-src_a)/0xFF);
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format("RedS  %+08x -> %+08x") % (uint32_t)(src & Color::RED_MASK) % ((src & Color::RED_MASK)*src_a/0xFF);
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << boost::format("  %+08x + %+08x = %+08x   red=%+08x green=%+08x blue=%+08x") % dst % (uint32_t)src % (alpha | red | green | blue) % red % green % blue;

    dst = alpha | red | green | blue;

    return dst;
}


std::ostream &operator<<(std::ostream &os, const Color &color) {
    return os << boost::format("%+02x%+02x%+02x%+02x") % (uint32_t)color.red() % (uint32_t)color.green() % (uint32_t)color.blue() % (uint32_t)color.alpha();
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
