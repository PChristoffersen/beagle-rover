#include "component.h"

Component::Component() : 
    m_initialized(false)
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