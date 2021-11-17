#ifndef _LEDCOLORLAYER_H_
#define _LEDCOLORLAYER_H_

#include <memory>

#include "ledcolor.h"
#include <robotcontrolext.h>


constexpr unsigned int LED_PIXEL_COUNT { RC_EXT_NEOPIXEL_COUNT };

using LEDColorArray = std::array<LEDColor, LED_PIXEL_COUNT>;
using LEDRawColorArray = std::array<LEDColor::raw_type, LED_PIXEL_COUNT>;


class LEDColorLayer : public LEDColorArray, public std::enable_shared_from_this<LEDColorLayer> {
    public:
        LEDColorLayer(int depth);

        void setVisible(bool visible);

        int depth() const { return m_depth; }
        bool visible() const { return m_visible; }

    private:
        int m_depth;
        bool m_visible;
};


LEDColorArray &operator+=(LEDColorArray &dst, const LEDColorLayer &layer);
LEDRawColorArray &operator+=(LEDRawColorArray &dst, const LEDColorLayer &layer);

#endif
