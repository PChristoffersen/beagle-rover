

#include "ledcontrol.h"
#include "ledanimation.h"


using namespace boost;
using namespace boost::asio;

LEDAnimation::LEDAnimation(io_context &io, shared_ptr<LEDControl> control) :
    m_control(control),
    m_timer(io)
{

}


void LEDAnimation::start() {

}


void LEDAnimation::stop() {
    m_timer.cancel();
}
