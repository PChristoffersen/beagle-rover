#ifndef _ROBOT_LED_COLOR_H_
#define _ROBOT_LED_COLOR_H_

#include <cstdint>
#include <string>
#include <array>
#include <limits>
#include <algorithm>
#include <iostream>
#include <boost/format.hpp>


namespace Robot::LED {

    /**
     * @brief Color class representing a single LED color
     */
    class Color {
        public:
            using raw_type        = std::uint32_t;
            using channel_type    = std::uint8_t;
            using brightness_type = float;

            enum class Correction : raw_type;

            static constexpr auto     ALPHA_SHIFT { 24 };
            static constexpr raw_type ALPHA_MASK  { 0xFF000000 };
            static constexpr auto     GREEN_SHIFT { 16 };
            static constexpr raw_type GREEN_MASK  { 0x00FF0000 };
            static constexpr auto     RED_SHIFT   { 8 };
            static constexpr raw_type RED_MASK    { 0x0000FF00 };
            static constexpr auto     BLUE_SHIFT  { 0 };
            static constexpr raw_type BLUE_MASK   { 0x000000FF };
            static constexpr raw_type CHANNEL_MASK { 0xFF };
            static constexpr channel_type CHANNEL_MAX { std::numeric_limits<channel_type>::max() };
            static constexpr channel_type CHANNEL_MIN { std::numeric_limits<channel_type>::min() };

            static constexpr brightness_type BRIGHTNESS_MAX { 1.0f };
            static constexpr brightness_type BRIGHTNESS_MIN { 0.0f };
            static constexpr brightness_type BRIGHTNESS_DEFAULT { BRIGHTNESS_MAX };

            static const Color TRANSPARENT;
            static const Color BLACK;
            static const Color WHITE;
            static const Color RED;
            static const Color GREEN;
            static const Color BLUE;

            constexpr Color() : Color(TRANSPARENT) {}
            constexpr Color(const Color &color) : m_data{ color.m_data } {}
            constexpr Color(const Color &&color) : m_data{ color.m_data } {}
            constexpr Color(channel_type r, channel_type g, channel_type b) : m_data { raw_argb(CHANNEL_MAX, r, g, b) } {}
            constexpr Color(channel_type r, channel_type g, channel_type b, channel_type a) : m_data { raw_argb(a, r, g, b) } {}
            constexpr Color(int r, int g, int b) : m_data { raw_argb(CHANNEL_MAX, to_channel(r), to_channel(g), to_channel(b)) } {}
            constexpr Color(int r, int g, int b, int a) : m_data { raw_argb(to_channel(a), to_channel(r), to_channel(g), to_channel(b)) } {}
            constexpr Color(unsigned int r, unsigned int g, unsigned int b) : m_data { raw_argb(CHANNEL_MAX, to_channel(r), to_channel(g), to_channel(b)) } {}
            constexpr Color(unsigned int r, unsigned int g, unsigned int b, unsigned int a) : m_data { raw_argb(to_channel(a), to_channel(r), to_channel(g), to_channel(b)) } {}
            constexpr Color(float r, float g, float b) : m_data { raw_argb(CHANNEL_MAX, to_channel(r), to_channel(g), to_channel(b)) } {}
            constexpr Color(float r, float g, float b, float a) : m_data { raw_argb(to_channel(a), to_channel(r), to_channel(g), to_channel(b)) } {}
            constexpr explicit Color(Correction correction) : m_data { static_cast<raw_type>(correction) } {}
            constexpr explicit Color(raw_type raw) : m_data { raw } {}
            Color(const std::string &value);

            constexpr Color opaque() const { return Color { m_data | ALPHA_MASK }; }

            static constexpr inline brightness_type clampBrightness(brightness_type b) { return std::clamp(b, BRIGHTNESS_MIN, BRIGHTNESS_MAX); }

            constexpr inline channel_type red()   const { return (m_data >> RED_SHIFT) & CHANNEL_MASK; }
            constexpr inline channel_type green() const { return (m_data >> GREEN_SHIFT) & CHANNEL_MASK; }
            constexpr inline channel_type blue()  const { return (m_data >> BLUE_SHIFT) & CHANNEL_MASK; }
            constexpr inline channel_type alpha() const { return (m_data >> ALPHA_SHIFT) & CHANNEL_MASK; }
            constexpr inline raw_type     raw()   const { return m_data; }

            constexpr inline void setRed(channel_type r)   { m_data = (m_data & ~RED_MASK)   | (static_cast<raw_type>(r)<<RED_SHIFT); }
            constexpr inline void setGreen(channel_type g) { m_data = (m_data & ~GREEN_MASK) | (static_cast<raw_type>(g)<<GREEN_SHIFT); }
            constexpr inline void setBlue(channel_type b)  { m_data = (m_data & ~BLUE_MASK)  | (static_cast<raw_type>(b)<<BLUE_SHIFT); }
            constexpr inline void setAlpha(channel_type a) { m_data = (m_data & ~ALPHA_MASK) | (static_cast<raw_type>(a)<<ALPHA_SHIFT); }

            std::string toString() const;
            std::string toStringRGB() const;
            std::string toStringRGBA() const;
            operator std::string() const { return toString(); }

            static constexpr inline raw_type rawRGB(channel_type r, channel_type g, channel_type b) { return raw_argb(CHANNEL_MAX,r,g,b); }
            static constexpr inline raw_type rawRGBA(channel_type r, channel_type g, channel_type b, channel_type a) { return raw_argb(a,r,g,b); }
            static constexpr inline channel_type rawRed(raw_type c) { return (c >> RED_SHIFT) & CHANNEL_MASK; }
            static constexpr inline channel_type rawGreen(raw_type c) { return (c >> GREEN_SHIFT) & CHANNEL_MASK; }
            static constexpr inline channel_type rawBlue(raw_type c) { return (c >> BLUE_SHIFT) & CHANNEL_MASK; }
            static constexpr inline channel_type rawAlpha(raw_type c) { return (c >> ALPHA_SHIFT) & CHANNEL_MASK; }

            Color &operator=(const Color &other) { m_data = other.m_data; return *this; }
            Color &operator=(const raw_type other) { m_data = other; return *this; }
            Color &operator=(const std::string &value) { m_data = (Color { value }).m_data; return *this; }

            constexpr bool operator==(const Color &other) const { return m_data==other.m_data; }
            constexpr bool operator==(const raw_type other) const { return m_data==other; }
            constexpr bool operator!=(const Color &other) const { return m_data!=other.m_data; }
            constexpr bool operator!=(const raw_type other) const { return m_data!=other; }

            Color &operator*=(const brightness_type brightness);
            Color  operator*(const brightness_type brightness) const;
            Color &operator*=(const Correction other);
            Color  operator*(const Correction other) const;
            Color &operator|(const Color &other) { m_data |= other.m_data; return *this; }
            Color  operator|(const Color &other) const { return Color { m_data|other.m_data }; }
            Color &operator&(const Color &other) { m_data &= other.m_data; return *this; }
            Color  operator&(const Color &other) const { return Color { m_data&other.m_data }; }

            /**
             * @brief Blend this color with color
             * 
             * res.r = dst.r * (1 - src.a) + src.r * src.a
             * res.g = dst.g * (1 - src.a) + src.g * src.a
             * res.b = dst.b * (1 - src.a) + src.b * src.a
             * res.a = dst.a * (1 - src.a) + src.a
             * 
             * @param color The color to blend with
             * @return Color& this
             */
            Color &operator<<(const Color &color);



            class HSV {
                public:
                    constexpr HSV(std::uint8_t h, std::uint8_t s, std::uint8_t v) : m_hue { h }, m_sat { s }, m_val { v } {}

                    /**
                     * @brief Convert a hue, saturation, and value to RGB
                     * 
                     * using a visually balanced rainbow (vs a straight
                     * mathematical spectrum).
                     * This 'rainbow' yields better yellow and orange
                     * than a straight 'spectrum'.
                     * 
                     * @note Here hue is 0-255, not just 0-191
                     * 
                     * @return Color Converted RGB Color
                     */
                    Color rainbow() const;

                    /**
                     * @brief Convert a hue, saturation, and value to RGB
                     * 
                     * Using a mathematically straight spectrum (vs
                     * a visually balanced rainbow).
                     * This 'spectrum' will have more green & blue
                     * than a 'rainbow', and less yellow and orange.
                     * 
                     * @note Here hue is 0-255, not just 0-191
                     * 
                     * @return Color Converted RGB Color
                     */
                    Color spectrum() const;

                    /**
                     * @brief Convert hue, saturation, and value to RGB.
                     * 
                     * This 'spectrum' conversion will be more green & blue
                     * than a real 'rainbow', and the hue is specified just
                     * in the range 0-191.  Together, these result in a
                     * slightly faster conversion speed, at the expense of
                     * color balance.
                     * 
                     * @note Hue is 0-191 only! Saturation & value are 0-255 each.
                     * 
                     * @return Color Converted RGB Color
                     */
                    Color raw() const;

                    operator Color() const { return rainbow(); }

                private:
                    std::uint8_t m_hue;
                    std::uint8_t m_sat;
                    std::uint8_t m_val;
            };


        private:
            raw_type m_data;

            static constexpr inline raw_type raw_argb(raw_type a, raw_type r, raw_type g, raw_type b)
            {
                return a<<ALPHA_SHIFT | r<<RED_SHIFT | g<<GREEN_SHIFT | b<<BLUE_SHIFT;
            }

            static constexpr inline channel_type to_channel(float v) { return std::clamp<float>(v*static_cast<float>(CHANNEL_MAX)+0.5f, CHANNEL_MIN, CHANNEL_MAX); }
            static constexpr inline channel_type to_channel(int v) { return std::clamp<int>(v, CHANNEL_MIN, CHANNEL_MAX); }
            static constexpr inline channel_type to_channel(unsigned int v) { return std::clamp<unsigned int>(v, CHANNEL_MIN, CHANNEL_MAX); }

            friend std::ostream &operator<<(std::ostream &os, const Color &color)
            {
                return os << "Color<" << color.toString() << ">";
            }
    };






    constexpr Color Color::TRANSPARENT { CHANNEL_MIN, CHANNEL_MIN, CHANNEL_MIN, CHANNEL_MIN };
    constexpr Color Color::BLACK       { CHANNEL_MIN, CHANNEL_MIN, CHANNEL_MIN };
    constexpr Color Color::WHITE       { CHANNEL_MAX, CHANNEL_MAX, CHANNEL_MAX };
    constexpr Color Color::RED         { CHANNEL_MAX, CHANNEL_MIN, CHANNEL_MIN };
    constexpr Color Color::GREEN       { CHANNEL_MIN, CHANNEL_MAX, CHANNEL_MIN };
    constexpr Color Color::BLUE        { CHANNEL_MIN, CHANNEL_MIN, CHANNEL_MAX };


    /**
     * @brief LED Color correction values
     * 
     * Values borrowed from the FastLED library
     */
    enum class Color::Correction : Color::raw_type {
        /// typical values for SMD5050 LEDs
        TypicalSMD5050 = Color(0xFF, 0xB0, 0xF0).raw()  /* 255, 176, 240 */,
        /// typical values for generic LED strips
        TypicalLEDStrip = Color(0xFF, 0xB0, 0xF0).raw() /* 255, 176, 240 */,

        /// typical values for 8mm "pixels on a string"
        /// also for many through-hole 'T' package LEDs
        Typical8mmPixel = Color(0xFF, 0xE0, 0x8C).raw() /* 255, 224, 140 */,
        TypicalPixelString = Color(0xFF, 0xE0, 0x8C).raw() /* 255, 224, 140 */,

        /// uncorrected color
        UncorrectedColor = Color(0xFF, 0xFF, 0xFF).raw()
    };



    /**
     * @brief An std::array of colors
     * 
     * An std::array of Color with additional operators for applying brightness and color correction.
     * 
     * @tparam NCOLORS Number of colors in the array
     */
    template<std::size_t NCOLORS>
    class ColorArray : public std::array<Color, NCOLORS> {
        public:
            using std::array<Color, NCOLORS>::array;

            /**
             * @brief Adjust the brightness of all colors in the array
             * 
             * @param brightness Brightness 0.0 - 1.0
             */
            ColorArray<NCOLORS> &operator*=(const Color::brightness_type brightness)
            {
                std::for_each(this->begin(), this->end(), [brightness](auto &dst) { dst *= brightness; });
                return *this;
            }

            /**
             * @brief Apply color correction to all the colors in the array
             * 
             * @param correction Correction to apply
             */
            ColorArray<NCOLORS> &operator*=(const Color::Correction correction)
            {
                std::for_each(this->begin(), this->end(), [correction](auto &dst) { dst *= correction; });
                return *this;
            }
            /**
             * @brief Blend all the colors from another array into this
             * 
             * @param other Array to blend
             */
            ColorArray<NCOLORS> &operator<<(const ColorArray<NCOLORS> &other)
            {
                std::transform(this->begin(), this->end(), other.begin(), this->begin(), [](auto &dst, const auto &src){ return dst<<src; });
                return *this;
            }
    };

    /**
     * @brief An std::array of colors
     * 
     * @tparam NCOLORS 
     */
    template<std::size_t NCOLORS>
    class RawColorArray : public std::array<Color::raw_type, NCOLORS> {
        public:
            using std::array<Color::raw_type, NCOLORS>::array;

            /**
             * @brief Construct a new RawColorArray from a ColorArray
             * 
             * @param array Source ColorArray
             */
            RawColorArray(const ColorArray<NCOLORS> &array)
            {
                std::transform(this->begin(), this->end(), array.begin(), this->begin(), [](auto &dst, const auto &src){ return (dst = src.raw()); });
            }
    };




}



#endif
