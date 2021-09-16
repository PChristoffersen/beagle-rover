#ifndef _LED_ANIMATION_H_
#define _LED_ANIMATION_H_

#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

class LEDAnimation {
    public:
        LEDAnimation(boost::asio::io_context &io, boost::shared_ptr<class LEDControl> control);

        virtual void start();
        virtual void stop();
    
    protected:
        boost::weak_ptr<class LEDControl> m_control;
        boost::asio::steady_timer m_timer;

        virtual void timer()=0;
};

#endif
