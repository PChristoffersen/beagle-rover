#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include <cstdint>
#include <memory>
#include <boost/asio.hpp>

#include <robotcontrolext.h>

#include "ledcolor.h"

class LEDControl : public std::enable_shared_from_this<LEDControl> {
    public:
        explicit LEDControl(std::shared_ptr<class RobotContext> context);
        LEDControl(const LEDControl&) = delete; // No copy constructor
        LEDControl(LEDControl&&) = delete; // No move constructor
        virtual ~LEDControl();

        void init();
        void cleanup();

        constexpr int count() const { return LED_PIXEL_COUNT; }

        void updatePixels(LEDColorList const &pixels);
        void setAll(const LEDColor &color);

    protected:
        friend class LEDAnimation;

        void _updatePixels(std::uint32_t pixels[]);

    private:
        static constexpr int LED_PIXEL_COUNT { RC_EXT_NEOPIXEL_COUNT };
        bool m_initialized;
};

#endif
