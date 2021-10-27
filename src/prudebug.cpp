#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrolext.h>

#include "prudebug.h"
#include "robotcontext.h"

using namespace std;


static constexpr auto TIMER_INTERVAL = chrono::milliseconds(100);


PRUDebug::PRUDebug(shared_ptr<RobotContext> context) :
    m_initialized { false },
    m_timer { context->io() }
{
}


PRUDebug::~PRUDebug() 
{
    cleanup();
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void PRUDebug::init() 
{
    rc_ext_debug_init();

    m_timer.expires_after(TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&PRUDebug::timer, this, _1));

    m_initialized = true;
}


void PRUDebug::cleanup() 
{
    if (!m_initialized)
        return;
    m_initialized = false;
    m_timer.cancel();
    rc_ext_debug_cleanup();
}


void PRUDebug::timer(boost::system::error_code error) 
{
    if (error!=boost::system::errc::success || !m_initialized) {
        return;
    }

    volatile const char *msg = nullptr;
    while (msg=rc_ext_debug_next()) {
        BOOST_LOG_TRIVIAL(debug) << msg;
    }

    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&PRUDebug::timer, this, _1));
}
