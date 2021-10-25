#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include <stdint.h>
#include <memory>
#include <boost/asio.hpp>
#include "ledcolor.h"

class LEDControl : public std::enable_shared_from_this<LEDControl> {
    public:
        LEDControl(std::shared_ptr<class RobotContext> context);
        virtual ~LEDControl();

        void init();
        void cleanup();

        int count() const { return LED_PIXEL_COUNT; }

        void updatePixels(LEDColorList const &pixels);
        void setAll(const LEDColor &color);

    protected:
        friend class LEDAnimation;

        void _updatePixels(uint32_t pixels[]);

    private:
        static const int LED_PIXEL_COUNT;
        bool m_initialized;
};

#endif
