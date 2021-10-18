#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrol-ext.h>

#include "prudebug.h"
#include "robotcontext.h"

using namespace std::chrono;
using namespace boost;
using namespace boost::asio;

#define TIMER_INTERVAL milliseconds(100)


PRUDebug::PRUDebug(shared_ptr<RobotContext> context) :
    m_timer(*(context->io()))
{
}

void PRUDebug::init() {
    rc_ext_debug_init();

    m_timer.expires_after(TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&PRUDebug::timer, this));
}


void PRUDebug::cleanup() {
    m_timer.cancel();
    rc_ext_debug_cleanup();
}


void PRUDebug::timer() {
    volatile const char *msg = NULL;
    while (msg=rc_ext_debug_next()) {
        BOOST_LOG_TRIVIAL(debug) << msg;
    }

    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&PRUDebug::timer, this));
}
