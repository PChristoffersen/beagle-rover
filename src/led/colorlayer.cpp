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
}


void ColorLayer::setVisible(bool visible) 
{
    const guard lock(m_mutex);
    if (m_visible!=visible) {
        m_visible = visible;
        show();
    }
}


void ColorLayer::show()
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



RawColorArray &operator<<(RawColorArray &dst, ColorLayer &layer) {
    const ColorLayer::guard lock(layer.mutex());
    BOOST_LOG_TRIVIAL(trace) << __FUNCTION__ << " <<  layer=" << layer.depth();
    if (layer.visible()) {
        std::transform(dst.begin(), dst.end(), layer.begin(), dst.begin(), [](auto &&dst, auto &&src){ return dst<<src; });
    }
    return dst;
}


}