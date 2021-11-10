#include "ledanimation.h"

#include "ledcontrol.h"

using namespace std;


LEDAnimation::LEDAnimation(boost::asio::io_context &io, shared_ptr<LEDControl> control) :
    m_control { control },
    m_timer { io }
{

}


void LEDAnimation::start() 
{

}


void LEDAnimation::stop() 
{
    m_timer.cancel();
}
