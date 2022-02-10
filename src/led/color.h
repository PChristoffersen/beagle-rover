#ifndef _ROBOT_LED_COLOR_H_
#define _ROBOT_LED_COLOR_H_

#include <cstdint>
#include <array>
#include <iostream>


namespace Robot::LED {

    class Color {
        public:
            static constexpr auto     ALPHA_SHIFT { 24 };
            static constexpr uint32_t ALPHA_MASK  { 0x00FF0000 };
            static constexpr auto     GREEN_SHIFT { 16 };
            static constexpr uint32_t GREEN_MASK  { 0x00FF0000 };
            static constexpr auto     RED_SHIFT   { 8 };
            static constexpr uint32_t RED_MASK    { 0x0000FF00 };
            static constexpr auto     BLUE_SHIFT  { 0 };
            static constexpr uint32_t BLUE_MASK   { 0x000000FF };

            using raw_type = std::uint32_t;

            constexpr Color() : Color(TRANSPARENT) {}
            constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b) : m_data { raw_argb(0xFF, r, g, b) } {}
            constexpr Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint32_t a) : m_data { raw_argb(a, r, g, b) } {}
            constexpr Color(const Color &color) : m_data{ color.m_data } {}
            constexpr Color(const Color &&color) : m_data{ color.m_data } {}

            inline std::uint8_t red() const { return (m_data >> RED_SHIFT) & 0xFF; }
            inline std::uint8_t green() const { return (m_data >> GREEN_SHIFT) & 0xFF; }
            inline std::uint8_t blue() const { return (m_data >> BLUE_SHIFT) & 0xFF; }
            inline std::uint8_t alpha() const { return (m_data >> ALPHA_SHIFT) & 0xFF; }
            inline raw_type     data() const { return m_data; }

            static constexpr inline std::uint8_t rawRed(raw_type c) { return (c >> RED_SHIFT) & 0xFF; }
            static constexpr inline std::uint8_t rawGreen(raw_type c) { return (c >> GREEN_SHIFT) & 0xFF; }
            static constexpr inline std::uint8_t rawBlue(raw_type c) { return (c >> BLUE_SHIFT) & 0xFF; }
            static constexpr inline std::uint8_t rawAlpha(raw_type c) { return (c >> ALPHA_SHIFT) & 0xFF; }

            Color &operator=(const Color &other) { m_data = other.m_data; return *this; }
            Color &operator=(const raw_type other) { m_data = other; return *this; }

            operator raw_type() const { return m_data; }

            static const Color TRANSPARENT;
            static const Color BLACK;
            static const Color WHITE;
            static const Color RED;
            static const Color GREEN;
            static const Color BLUE;

        private:
            raw_type m_data;

            static constexpr inline raw_type raw_argb(raw_type a, raw_type r, raw_type g, raw_type b)
            {
                return a<<ALPHA_SHIFT | r<<RED_SHIFT | g<<GREEN_SHIFT | b<<BLUE_SHIFT;
            }

            friend raw_type &operator<<(raw_type &value, const Color &color);
            friend std::ostream &operator<<(std::ostream &os, const Color &color);
    };


    constexpr Color Color::TRANSPARENT { 0x00, 0x00, 0x00, 0x00 };
    constexpr Color Color::BLACK       { 0x00, 0x00, 0x00 };
    constexpr Color Color::WHITE       { 0xFF, 0xFF, 0xFF };
    constexpr Color Color::RED         { 0xFF, 0x00, 0x00 };
    constexpr Color Color::GREEN       { 0x00, 0xFF, 0x00 };
    constexpr Color Color::BLUE        { 0x00, 0x00, 0xFF };

};



#endif
