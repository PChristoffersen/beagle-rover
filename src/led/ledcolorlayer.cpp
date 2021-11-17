#include "ledcolorlayer.h"

#include <algorithm>
#include <boost/log/trivial.hpp>

using namespace std;


LEDColorLayer::LEDColorLayer(int depth) :
    m_depth { depth },
    m_visible { true }
{
}


void LEDColorLayer::setVisible(bool visible) {
    m_visible = visible;
}


LEDColorArray &operator+=(LEDColorArray &dst, const LEDColorLayer &layer) 
{
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " COL  layer=" << layer.depth();
    if (layer.visible()) {
        transform(dst.begin(), dst.end(), layer.begin(), dst.begin(), plus<>());
    }
    return dst;
}


LEDRawColorArray &operator+=(LEDRawColorArray &dst, const LEDColorLayer &layer) 
{
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " RAW  layer=" << layer.depth();
    if (layer.visible()) {
        transform(dst.begin(), dst.end(), layer.begin(), dst.begin(), plus<>());
    }
    return dst;
}
