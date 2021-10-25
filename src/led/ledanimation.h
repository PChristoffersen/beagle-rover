#ifndef _LED_ANIMATION_H_
#define _LED_ANIMATION_H_

#include <stdint.h>
#include <memory>
#include <boost/asio.hpp>


class LEDAnimation : public std::enable_shared_from_this<LEDAnimation> {
    public:
        LEDAnimation(boost::asio::io_context &io, std::shared_ptr<class LEDControl> control);

        virtual void start();
        virtual void stop();
    
    protected:
        std::weak_ptr<class LEDControl> m_control;
        boost::asio::steady_timer m_timer;

        virtual void timer()=0;
};

#endif
