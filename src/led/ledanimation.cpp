

#include "ledcontrol.h"
#include "ledanimation.h"


LEDAnimation::LEDAnimation(boost::asio::io_context &io, std::shared_ptr<LEDControl> control) :
    m_control(control),
    m_timer(io)
{

}


void LEDAnimation::start() {

}


void LEDAnimation::stop() {
    m_timer.cancel();
}
