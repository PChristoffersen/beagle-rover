#include "prudebug.h"

#include <iostream>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#include <robotcontrolext.h>

using namespace std;

namespace Robot::PRU {
//#define DEBUG_ENABLED

static constexpr auto TIMER_INTERVAL { 100ms };



Debug::Debug(shared_ptr<Robot::Context> context) :
    m_initialized { false },
    m_timer { context->io() }
{
}


Debug::~Debug() 
{
    cleanup();
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


void Debug::init() 
{
    #ifdef DEBUG_ENABLED
    rc_ext_debug_init();

    m_timer.expires_after(TIMER_INTERVAL);
    timer_setup();

    m_initialized = true;
    #endif
}


void Debug::cleanup() 
{
    const lock_guard<mutex> lock(m_mutex);
    if (!m_initialized)
        return;
    m_initialized = false;
    m_timer.cancel();
    rc_ext_debug_cleanup();
}


void Debug::timer_setup() {
    m_timer.expires_at(m_timer.expiry() + TIMER_INTERVAL);
    m_timer.async_wait(
        [self_ptr=weak_from_this()] (auto &error) {
            if (auto self = self_ptr.lock()) { 
                self->timer(error); 
            }
        }
    );
}

void Debug::timer(boost::system::error_code error) 
{
    const lock_guard<mutex> lock(m_mutex);
    if (error!=boost::system::errc::success || !m_initialized) {
        return;
    }

    volatile const char *msg = nullptr;
    while (msg=rc_ext_debug_next()) {
        BOOST_LOG_TRIVIAL(debug) << msg;
    }

    timer_setup();
}

    
};
