#include "component.h"

using namespace boost;
using namespace boost::asio;

Component::Component(shared_ptr<io_context> io) : 
    m_initialized(false), 
    m_io(io)
{
}

Component::~Component() {
    if (m_initialized) {
        cleanup();
    }
}

void Component::init() {
    m_initialized = true;
}

void Component::cleanup() {
    m_initialized = false;
}