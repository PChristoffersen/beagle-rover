#include "colorlayer.h"

#include <algorithm>
#include <boost/log/trivial.hpp>

#include "control.h"


namespace Robot::LED {

ColorLayer::ColorLayer(uint depth) :
    m_depth { depth },
    m_visible { false },
    m_segments {
        Segment { this, 0, PIXEL_COUNT/2 },
        Segment { this, PIXEL_COUNT/2, PIXEL_COUNT/2 }, 
    }
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
}


ColorLayer::~ColorLayer() 
{
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__;
    if (auto control = m_control.lock()) {
        control->removeLayer(nullptr);
    }
}


void ColorLayer::setVisible(bool visible) 
{
    if (m_visible!=visible) {
        m_visible = visible;
    }
}


void ColorLayer::show()
{
    if (auto control = m_control.lock()) {
        control->show();
    }
}


void ColorLayer::detach() 
{
    if (auto control = m_control.lock()) {
        const auto &self = shared_from_this();
        control->removeLayer(self);
    }
    m_control.reset();
}


void ColorLayer::lock() 
{
    if (auto control = m_control.lock()) {
        control->lock();
    }
}


void ColorLayer::unlock() 
{
    if (auto control = m_control.lock()) {
        control->unlock();
    }
}


void ColorLayer::setControl(const std::shared_ptr<Control> &control) 
{
    m_control = control;
}



RawColorArray &operator<<(RawColorArray &dst, const ColorLayer &layer) {
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << " <<  layer=" << layer.depth();
    if (layer.visible()) {
        std::transform(dst.begin(), dst.end(), layer.begin(), dst.begin(), [](auto &&dst, auto &&src){ return dst<<src; });
    }
    return dst;
}


};