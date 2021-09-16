#ifndef _LED_CONTROL_H_
#define _LED_CONTROL_H_

#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <robotcontrol-ext.h>
#include "../component.h"


#define LED_PIXEL_COUNT RC_EXT_NEOPIXEL_COUNT

class LEDControl : public Component, public boost::enable_shared_from_this<LEDControl> {
    public:
        LEDControl(boost::asio::io_context &io);

        void init() override;
        void cleanup() override;

        int count() { return LED_PIXEL_COUNT; }

    protected:
        friend class LEDAnimation;

        void updatePixels(uint32_t pixels[LED_PIXEL_COUNT]);
        void setAll(uint8_t r, uint8_t g, uint8_t b);

    private:
        boost::asio::io_context &m_io;

};

#endif
