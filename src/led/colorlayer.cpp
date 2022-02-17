#include "colorlayer.h"

#include <algorithm>
#include <boost/log/trivial.hpp>

#include "control.h"


namespace Robot::LED {

ColorLayer::ColorLayer(const std::string &name, uint depth, bool internal) :
    m_name { name },
    m_depth { depth },
    m_internal { internal },
    m_visible { false },
    m_segments {
        Segment { this, "front", 0, PIXEL_COUNT/2 },
        Segment { this, "back", PIXEL_COUNT/2, PIXEL_COUNT/2 }, 
    }
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
        update();
    }
}


void ColorLayer::update()
{
    const guard lock(m_mutex);
    if (m_show_sig) {
        (*m_show_sig)();
    }
}


void ColorLayer::setSignal(const std::shared_ptr<::Robot::ASyncSignal> &sig)
{
    const guard lock(m_mutex);
    m_show_sig = sig;
}

void ColorLayer::clearSignal()
{
    const guard lock(m_mutex);
    m_show_sig = nullptr;
}



ColorLayer::array_type &operator<<(ColorLayer::array_type &dst, ColorLayer &layer) {
    const ColorLayer::guard lock(layer.mutex());
    //BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << " <<  layer=" << layer.depth();
    if (layer.visible()) {
        std::transform(dst.begin(), dst.end(), layer.begin(), dst.begin(), [](auto &&dst, auto &&src){ return dst<<src; });
    }
    return dst;
}



}