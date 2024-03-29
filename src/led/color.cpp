#include "color.h"

#include <cmath>
#include <algorithm>
#include <boost/log/trivial.hpp>
#include <boost/throw_exception.hpp>


namespace Robot::LED {



Color::Color(const std::string &value)
{
    auto len = value.length();
    if (len!=6 && len!=8) {
        BOOST_THROW_EXCEPTION(std::invalid_argument("Invalid color string length '"+value+"'"));
    }
    uint32_t v;
    std::size_t end = 0;
    if (len==6) { // Without alpha
        v = (std::stoul(value, &end, 16) << 8 ) | 0xFF;
    }
    else { // With alpha
        v = std::stoul(value, &end, 16);
    }
    if (end!=len) {
        BOOST_THROW_EXCEPTION(std::invalid_argument("Invalid color string '"+value+"'"));
    }
    m_data = raw_argb(
        (v) & CHANNEL_MASK, 
        (v>>24) & CHANNEL_MASK, 
        (v>>16) & CHANNEL_MASK,
        (v>>8) & CHANNEL_MASK
    );
}



std::string Color::toString() const
{
    if (alpha()!=CHANNEL_MAX) {
        return toStringRGBA();
    }
    return toStringRGB();
}

std::string Color::toStringRGB() const
{
    return (boost::format("%+02X%+02X%+02X") % static_cast<int>(red()) % static_cast<int>(green()) % static_cast<int>(blue())).str();
}

std::string Color::toStringRGBA() const
{
    return (boost::format("%+02X%+02X%+02X%+02X") % static_cast<int>(red()) % static_cast<int>(green()) % static_cast<int>(blue()) % static_cast<int>(alpha())).str();
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




/**
 * @brief HSV Conversion adapted from the FastLED library
 * 
 * https://github.com/FastLED/FastLED
 * 
 * 
 * The MIT License (MIT)
 * 
 * Copyright (c) 2013 FastLED
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */


/**
 * @brief 
 * 
 * @param i 
 * @param scale 
 * @return constexpr std::uint8_t 
 */
static constexpr inline std::uint8_t scale8(std::uint8_t i, std::uint8_t scale)
{
    using std::uint16_t;
    return (((uint16_t)i) * (1+(uint16_t)(scale))) >> 8;
}

static constexpr inline std::uint8_t scale8_video(std::uint8_t i, std::uint8_t scale)
{
    using std::uint8_t;
    uint8_t j = (((int)i * (int)scale) >> 8) + ((i&&scale)?1:0);
    // uint8_t nonzeroscale = (scale != 0) ? 1 : 0;
    // uint8_t j = (i == 0) ? 0 : (((int)i * (int)(scale) ) >> 8) + nonzeroscale;
    return j;
}


Color Color::HSV::rainbow() const
{
    using std::uint8_t;

    constexpr auto K255 { 255 };
    constexpr auto K171 { 171 };
    constexpr auto K170 { 170 };
    constexpr auto K85  { 85 };

    // Yellow has a higher inherent brightness than
    // any other color; 'pure' yellow is perceived to
    // be 93% as bright as white.  In order to make
    // yellow appear the correct relative brightness,
    // it has to be rendered brighter than all other
    // colors.
    // Level Y1 is a moderate boost, the default.
    // Level Y2 is a strong boost.
    const uint8_t Y1 = 1;
    const uint8_t Y2 = 0;
    
    // G2: Whether to divide all greens by two.
    // Depends GREATLY on your particular LEDs
    const uint8_t G2 = 0;
    
    // Gscale: what to scale green down by.
    // Depends GREATLY on your particular LEDs
    const uint8_t Gscale = 0;
    
    uint8_t hue = m_hue;
    uint8_t sat = m_sat;
    uint8_t val = m_val;

    uint8_t offset = m_hue & 0x1F; // 0..31
    
    // offset8 = offset * 8
    uint8_t offset8 = offset;
    // On ARM and other non-AVR platforms, we just shift 3.
    offset8 <<= 3;
    
    uint8_t third = scale8( offset8, (256 / 3)); // max = 85
    
    uint8_t r, g, b;
    
    if( ! (hue & 0x80) ) {
        // 0XX
        if( ! (hue & 0x40) ) {
            // 00X
            //section 0-1
            if( ! (hue & 0x20) ) {
                // 000
                //case 0: // R -> O
                r = K255 - third;
                g = third;
                b = 0;
            } else {
                // 001
                //case 1: // O -> Y
                if( Y1 ) {
                    r = K171;
                    g = K85 + third ;
                    b = 0;
                }
                if( Y2 ) {
                    r = K170 + third;
                    //uint8_t twothirds = (third << 1);
                    uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                    g = K85 + twothirds;
                    b = 0;
                }
            }
        } else {
            //01X
            // section 2-3
            if( !  (hue & 0x20) ) {
                // 010
                //case 2: // Y -> G
                if( Y1 ) {
                    //uint8_t twothirds = (third << 1);
                    uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                    r = K171 - twothirds;
                    g = K170 + third;
                    b = 0;
                }
                if( Y2 ) {
                    r = K255 - offset8;
                    g = K255;
                    b = 0;
                }
            } else {
                // 011
                // case 3: // G -> A
                r = 0;
                g = K255 - third;
                b = third;
            }
        }
    } else {
        // section 4-7
        // 1XX
        if( ! (hue & 0x40) ) {
            // 10X
            if( ! ( hue & 0x20) ) {
                // 100
                //case 4: // A -> B
                r = 0;
                //uint8_t twothirds = (third << 1);
                uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                g = K171 - twothirds; //K170?
                b = K85  + twothirds;
                
            } else {
                // 101
                //case 5: // B -> P
                r = third;
                g = 0;
                b = K255 - third;
                
            }
        } else {
            if( !  (hue & 0x20)  ) {
                // 110
                //case 6: // P -- K
                r = K85 + third;
                g = 0;
                b = K171 - third;
                
            } else {
                // 111
                //case 7: // K -> R
                r = K170 + third;
                g = 0;
                b = K85 - third;
            }
        }
    }
    
    // This is one of the good places to scale the green down,
    // although the client can scale green down as well.
    if( G2 ) g = g >> 1;
    if( Gscale ) g = scale8_video( g, Gscale);
    
    // Scale down colors if we're desaturated at all
    // and add the brightness_floor to r, g, and b.
    if( sat != 255 ) {
        if( sat == 0) {
            r = 255; b = 255; g = 255;
        } else {
            uint8_t desat = 255 - sat;
            desat = scale8_video( desat, desat);

            uint8_t satscale = 255 - desat;
            //satscale = sat; // uncomment to revert to pre-2021 saturation behavior

            //nscale8x3_video( r, g, b, sat);
            if( r ) r = scale8( r, satscale) + 1;
            if( g ) g = scale8( g, satscale) + 1;
            if( b ) b = scale8( b, satscale) + 1;
            uint8_t brightness_floor = desat;
            r += brightness_floor;
            g += brightness_floor;
            b += brightness_floor;
        }
    }
    
    // Now scale everything down if we're at value < 255.
    if( val != 255 ) {
        
        val = scale8_video( val, val);
        if( val == 0 ) {
            r=0; g=0; b=0;
        } else {
            // nscale8x3_video( r, g, b, val);
            if( r ) r = scale8( r, val) + 1;
            if( g ) g = scale8( g, val) + 1;
            if( b ) b = scale8( b, val) + 1;
        }
    }
    
    return Color { r, g, b };
}


Color Color::HSV::spectrum() const
{
    HSV hsv { scale8(m_hue, 191), m_sat, m_val };
    return hsv.raw();
}


#define APPLY_DIMMING(X) (X)

Color Color::HSV::raw() const 
{
    //constexpr auto HSV_SECTION_6 { 0x20 };
    constexpr auto HSV_SECTION_3 { 0x40 };

    // Convert hue, saturation and brightness ( HSV/HSB ) to RGB
    // "Dimming" is used on saturation and brightness to make
    // the output more visually linear.

    // Apply dimming curves
    uint8_t value = APPLY_DIMMING(m_val);
    uint8_t saturation = m_sat;

    // The brightness floor is minimum number that all of
    // R, G, and B will be set to.
    uint8_t invsat = APPLY_DIMMING( 255 - saturation);
    uint8_t brightness_floor = (value * invsat) / 256;

    // The color amplitude is the maximum amount of R, G, and B
    // that will be added on top of the brightness_floor to
    // create the specific hue desired.
    uint8_t color_amplitude = value - brightness_floor;

    // Figure out which section of the hue wheel we're in,
    // and how far offset we are withing that section
    uint8_t section = m_hue / HSV_SECTION_3; // 0..2
    uint8_t offset = m_hue % HSV_SECTION_3;  // 0..63

    uint8_t rampup = offset; // 0..63
    uint8_t rampdown = (HSV_SECTION_3 - 1) - offset; // 63..0

    // We now scale rampup and rampdown to a 0-255 range -- at least
    // in theory, but here's where architecture-specific decsions
    // come in to play:
    // To scale them up to 0-255, we'd want to multiply by 4.
    // But in the very next step, we multiply the ramps by other
    // values and then divide the resulting product by 256.
    // So which is faster?
    //   ((ramp * 4) * othervalue) / 256
    // or
    //   ((ramp    ) * othervalue) /  64
    // It depends on your processor architecture.
    // On 8-bit AVR, the "/ 256" is just a one-cycle register move,
    // but the "/ 64" might be a multicycle shift process. So on AVR
    // it's faster do multiply the ramp values by four, and then
    // divide by 256.
    // On ARM, the "/ 256" and "/ 64" are one cycle each, so it's
    // faster to NOT multiply the ramp values by four, and just to
    // divide the resulting product by 64 (instead of 256).
    // Moral of the story: trust your profiler, not your insticts.

    // Since there's an AVR assembly version elsewhere, we'll
    // assume what we're on an architecture where any number of
    // bit shifts has roughly the same cost, and we'll remove the
    // redundant math at the source level:

    //  // scale up to 255 range
    //  //rampup *= 4; // 0..252
    //  //rampdown *= 4; // 0..252

    // compute color-amplitude-scaled-down versions of rampup and rampdown
    uint8_t rampup_amp_adj   = (rampup   * color_amplitude) / (256 / 4);
    uint8_t rampdown_amp_adj = (rampdown * color_amplitude) / (256 / 4);

    // add brightness_floor offset to everything
    uint8_t rampup_adj_with_floor   = rampup_amp_adj   + brightness_floor;
    uint8_t rampdown_adj_with_floor = rampdown_amp_adj + brightness_floor;

    uint8_t r, g, b;
    if( section ) {
        if( section == 1) {
            // section 1: 0x40..0x7F
            r = brightness_floor;
            g = rampdown_adj_with_floor;
            b = rampup_adj_with_floor;
        } else {
            // section 2; 0x80..0xBF
            r = rampup_adj_with_floor;
            g = brightness_floor;
            b = rampdown_adj_with_floor;
        }
    } else {
        // section 0: 0x00..0x3F
        r = rampdown_adj_with_floor;
        g = rampup_adj_with_floor;
        b = brightness_floor;
    }

    return Color( r, g, b );
}




}
