#ifndef _LED_COLOR_H_
#define _LED_COLOR_H_

#include <cstdint>
#include <vector>


class LEDColor {
    public:
        LEDColor(std::uint8_t r, std::uint8_t g, std::uint8_t b);
        LEDColor(const LEDColor &color);

        std::uint8_t getRed() const { return m_red; }
        std::uint8_t getGreen() const { return m_green; }
        std::uint8_t getBlue() const { return m_blue; }

        LEDColor &operator=(const LEDColor &other);
        operator std::uint32_t() const;
    private:
        std::uint8_t m_red;
        std::uint8_t m_green;
        std::uint8_t m_blue;
};

extern LEDColor LED_BLACK;


using LEDColorList = std::vector<LEDColor>;


#endif
