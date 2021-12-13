#include "color.h"

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


};
