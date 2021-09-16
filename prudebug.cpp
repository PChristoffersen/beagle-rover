#include <iostream>
#include <boost/bind.hpp>
#include <boost/format.hpp>

#include <robotcontrol-ext.h>

#include "prudebug.h"

using namespace std;
using namespace std::chrono;
using namespace boost;


#define TIMER_INTERVAL milliseconds(100)


PRUDebug::PRUDebug(boost::asio::io_context &io) :
    m_timer(io)
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
    const char *msg = NULL;
    while (msg=rc_ext_debug_next()) {
        cout << msg << endl;
    }

    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(boost::bind(&PRUDebug::timer, this));
}
