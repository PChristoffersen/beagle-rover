#include "ledcolorlayer.h"

#include <algorithm>
#include <boost/log/trivial.hpp>

#include "ledcontrol.h"

using namespace std;
using namespace Robot::LED;

ColorLayer::ColorLayer(int depth) :
    m_depth { depth },
    m_visible { true }
{
}


ColorLayer::~ColorLayer() 
{
    detach();
}


void ColorLayer::setVisible(bool visible) 
{
    m_visible = visible;
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
        auto self = shared_from_this();
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



ColorArray &Robot::LED::operator+=(ColorArray &dst, const ColorLayer &layer) 
{
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " COL  layer=" << layer.depth();
    if (layer.visible()) {
        transform(dst.begin(), dst.end(), layer.begin(), dst.begin(), plus<>());
    }
    return dst;
}


RawColorArray &Robot::LED::operator+=(RawColorArray &dst, const ColorLayer &layer) 
{
    //BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << " RAW  layer=" << layer.depth();
    if (layer.visible()) {
        transform(dst.begin(), dst.end(), layer.begin(), dst.begin(), plus<>());
    }
    return dst;
}
