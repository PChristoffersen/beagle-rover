#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <robotcontrol-ext.h>
#include "ledcolor.h"

#define LED_PIXEL_COUNT RC_EXT_NEOPIXEL_COUNT

class LEDControl : public boost::enable_shared_from_this<LEDControl> {
    public:

        LEDControl(boost::shared_ptr<class RobotContext> context);

        void init();
        void cleanup();

        int count() const { return LED_PIXEL_COUNT; }

        void updatePixels(LEDColorList const &pixels);
        void setAll(const LEDColor &color);

    protected:
        friend class LEDAnimation;

        void _updatePixels(uint32_t pixels[LED_PIXEL_COUNT]);

    private:

};

#endif
