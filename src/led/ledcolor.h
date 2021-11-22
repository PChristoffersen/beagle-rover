#ifndef _LED_COLOR_H_
#define _LED_COLOR_H_

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

            Color();
            Color(raw_type data);
            Color(std::uint8_t r, std::uint8_t g, std::uint8_t b);
            Color(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint32_t a);
            Color(const Color &color);

            inline std::uint8_t red() const { return (m_data >> RED_SHIFT) & 0xFF; }
            inline std::uint8_t green() const { return (m_data >> GREEN_SHIFT) & 0xFF; }
            inline std::uint8_t blue() const { return (m_data >> BLUE_SHIFT) & 0xFF; }
            inline std::uint8_t alpha() const { return (m_data >> ALPHA_SHIFT) & 0xFF; }
            inline raw_type     data() const { return m_data; }

            Color &operator=(const Color &other);
            Color &operator=(const raw_type other);

            operator raw_type() const { return m_data; }

            static const Color TRANSPARENT;
            static const Color BLACK;
            static const Color WHITE;
            static const Color RED;
            static const Color GREEN;
            static const Color BLUE;

        private:
            raw_type m_data;

            static inline raw_type raw_argb(raw_type a, raw_type r, raw_type g, raw_type b);

            friend raw_type &operator<<(raw_type &value, const Color &color);
            friend std::ostream &operator<<(std::ostream &os, const Color &color);
    };


};



#endif
