#include <iostream>
#include <boost/log/trivial.hpp>
#include <robotcontrol.h>
#include <robotcontrol-ext.h>

#include "robotcontext.h"

using namespace boost;
using namespace boost::asio;


RobotContext::RobotContext() : 
    m_started(false),
    m_io(new io_context())
{
    rc_model();
}

RobotContext::~RobotContext() {
    stop();
}

void RobotContext::start() {
    BOOST_LOG_TRIVIAL(trace) << "Starting thread";
    m_thread.reset(new thread(bind(&io_context::run, m_io.get())));
    m_started = true;
}

void RobotContext::stop() {
    BOOST_LOG_TRIVIAL(trace) << __PRETTY_FUNCTION__;
    if (!m_started)
        return;
    
    BOOST_LOG_TRIVIAL(trace) << "Stopping thread";
    m_io->stop();
    m_thread->join();
    m_thread.reset();
    BOOST_LOG_TRIVIAL(trace) << "Thread stopped";

    m_started = false;
}


