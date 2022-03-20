#include "colorlayer.h"

#include <algorithm>
#include <boost/log/trivial.hpp>

#include "control.h"


namespace Robot::LED {

ColorLayer::ColorLayer(const std::string &name, uint depth, bool internal) :
    array_type { DEFAULT_SEGMENTS },
    m_name { name },
    m_depth { depth },
    m_internal { internal },
    m_visible { false }
{
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


ColorLayer::~ColorLayer() 
{
    //BOOST_LOG_TRIVIAL(trace) << *this << " " << __FUNCTION__ ;
}


void ColorLayer::setVisible(bool visible) 
{
    const guard lock(m_mutex);
    if (m_visible!=visible) {
        m_visible = visible;
        m_update_signal();
    }
}


void ColorLayer::update()
{
    const guard lock(m_mutex);
    if (m_visible) {
        m_update_signal();
    }
}


void ColorLayer::detach() 
{
    if (auto control = m_control.lock()) {
        control->detachLayer(shared_from_this());
    }
}


void ColorLayer::disconnect()
{
    m_control_connection.disconnect();
    m_control.reset();
}


void ColorLayer::connect(const std::shared_ptr<class Control> &control, update_signal::slot_type func)
{
    m_control_connection = m_update_signal.connect(func);
    m_control = control;
}


ColorLayer::array_type &operator<<(ColorLayer::array_type &dst, ColorLayer &layer) {
    const ColorLayer::guard lock(layer.mutex());
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " <<  layer=" << layer.depth();
    if (layer.visible()) {
        std::transform(dst.begin(), dst.end(), layer.begin(), dst.begin(), [](auto &&dst, auto &&src){ return dst<<src; });
    }
    return dst;
}



}