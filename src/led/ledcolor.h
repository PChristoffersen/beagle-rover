#ifndef _LED_COLOR_H_
#define _LED_COLOR_H_

#include <cstdint>
#include <array>

class LEDColor {
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

        LEDColor();
        LEDColor(raw_type data);
        LEDColor(std::uint8_t r, std::uint8_t g, std::uint8_t b);
        LEDColor(std::uint8_t r, std::uint8_t g, std::uint8_t b, std::uint32_t a);
        LEDColor(const LEDColor &color);

        inline std::uint8_t red() const { return (m_data >> RED_SHIFT) & 0xFF; }
        inline std::uint8_t green() const { return (m_data >> GREEN_SHIFT) & 0xFF; }
        inline std::uint8_t blue() const { return (m_data >> BLUE_SHIFT) & 0xFF; }
        inline std::uint8_t alpha() const { return (m_data >> ALPHA_SHIFT) & 0xFF; }
        inline raw_type     data() const { return m_data; }

        LEDColor &operator=(const LEDColor &other);
        LEDColor &operator=(const raw_type other);
        LEDColor operator+(const LEDColor &other) const;

        operator raw_type() const { return m_data; }

        static const LEDColor TRANSPARENT;
        static const LEDColor BLACK;
        static const LEDColor WHITE;
        static const LEDColor RED;
        static const LEDColor GREEN;
        static const LEDColor BLUE;

    private:
        raw_type m_data;

        static inline raw_type raw_argb(raw_type a, raw_type r, raw_type g, raw_type b);
};


LEDColor::raw_type operator+(LEDColor::raw_type raw, const LEDColor &color);



#endif
