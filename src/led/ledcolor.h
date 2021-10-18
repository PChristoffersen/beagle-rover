#ifndef _LED_COLOR_H_
#define _LED_COLOR_H_

#include <stdint.h>
#include <vector>


class LEDColor {
    public:
        LEDColor(uint8_t r, uint8_t g, uint8_t b);
        LEDColor(const LEDColor &color);

        uint8_t getRed() const { return m_red; }
        uint8_t getGreen() const { return m_green; }
        uint8_t getBlue() const { return m_blue; }

        LEDColor &operator=(const LEDColor &other);
        operator uint32_t() const;
    private:
        uint8_t m_red;
        uint8_t m_green;
        uint8_t m_blue;
};

extern LEDColor LED_BLACK;


typedef std::vector<LEDColor> LEDColorList;


#endif
