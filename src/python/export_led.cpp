#include <memory>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/utility/string_ref.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/object.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include <led/control.h>
#include <led/color.h>
#include <led/colorlayer.h>
#include "util.h"
#include "subscription.h"

namespace py = boost::python;

using Robot::LED::Color, Robot::LED::ColorLayer;

namespace Robot::Python {

static Color tuple2color(const py::tuple & value) 
{
    if (py::len(value)==3) {
        return Color { 
            py::extract<std::uint8_t>(value[0]), 
            py::extract<std::uint8_t>(value[1]), 
            py::extract<std::uint8_t>(value[2]) 
        };
    }
    else if (py::len(value)==4) {
        return Color { 
            py::extract<std::uint8_t>(value[0]), 
            py::extract<std::uint8_t>(value[1]), 
            py::extract<std::uint8_t>(value[2]),
            py::extract<std::uint8_t>(value[3]) 
        };
    }

    PyErr_SetString(PyExc_IndexError, "Invalid tuple dimensions");
    py::throw_error_already_set();
    throw std::runtime_error("unreachable");
}


static Color str2color(const std::string &value) 
{   
    auto len = value.length();
    if ((len!=7 && len!=9) || value[0]!='#') {
        PyErr_SetString(PyExc_IndexError, "Invalid color string");
        py::throw_error_already_set();
    }
    uint32_t v;
    std::size_t end = 0;
    if (len==7) { // Without alpha
        v = (std::stoul(value.substr(1), &end, 16) << 8 ) | 0xFF;
    }
    else { // With alpha
        v = std::stoul(value.substr(1), &end, 16);
    }
    if (end+1!=len) {
        PyErr_SetString(PyExc_IndexError, "Invalid color string");
        py::throw_error_already_set();
    }
    return Color((v>>24) & 0xFF, (v>>16) & 0xFF, (v>>8) & 0xFF, (v) & 0xFF);
}

static std::string color2str(const Color &color)
{
    std::stringstream stream;
    stream << boost::format("#%+02x%+02x%+02x%+02x") % (uint32_t)color.red() % (uint32_t)color.green() % (uint32_t)color.blue() % (uint32_t)color.alpha();
    return stream.str();
}


void checkIndex(const ColorLayer &l, uint index) 
{
    if (index >= l.size()) {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        py::throw_error_already_set();
    }
}

void checkIndex(const ColorLayer::Segment &l, uint index) 
{
    if (index >= l.size()) {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        py::throw_error_already_set();
    }
}



void export_led() 
{
    using Robot::LED::Control;
    using Robot::LED::AnimationMode, Robot::LED::IndicatorMode;

    py::enum_<AnimationMode>("LEDAnimation")
        .value("NONE", AnimationMode::NONE)
        .value("HEADLIGHTS", AnimationMode::HEADLIGHTS)
        .value("CONSTRUCTION", AnimationMode::CONSTRUCTION)
        .value("POLICE", AnimationMode::POLICE)
        .value("AMBULANCE", AnimationMode::AMBULANCE)
        .value("KNIGHT_RIDER", AnimationMode::KNIGHT_RIDER)
        .value("RAINBOW", AnimationMode::RAINBOW)
        ;

    py::enum_<IndicatorMode>("LEDIndicator")
        .value("NONE", IndicatorMode::NONE)
        .value("LEFT", IndicatorMode::LEFT)
        .value("RIGHT", IndicatorMode::RIGHT)
        .value("HAZARD", IndicatorMode::HAZARD)
        ;


   py::class_<ColorLayer::Segment, boost::noncopyable>("LEDSegment", py::no_init)
        .def("__getitem__", +[](const ColorLayer::Segment &segment, uint index){
            checkIndex(segment, index);
            return color2str(segment[index]);
        })
        .def("__setitem__", +[](ColorLayer::Segment &segment, uint index, const std::string &value) {
            checkIndex(segment, index);
            segment[index] = str2color(value);
        })
        .def("__setitem__", +[](ColorLayer::Segment &segment, uint index, const py::tuple &value) {
            checkIndex(segment, index);
            segment[index] = tuple2color(value);
        })
        .def("__len__", &ColorLayer::Segment::size)
        ;

    py::class_<ColorLayer::SegmentArray, boost::noncopyable>("LEDSegmentArray", py::no_init)
        .def("__getitem__", +[](const ColorLayer::SegmentArray &array, uint index) {
            if (index >= array.size()) {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                py::throw_error_already_set();
            }
            return &array[index];
        }, py::return_internal_reference<>())
        .def("__len__", &ColorLayer::SegmentArray::size)
        ;

   py::class_<ColorLayer, std::shared_ptr<ColorLayer>, boost::noncopyable>("LEDColorLayer", py::init<int>())
        .add_property("depth", &ColorLayer::depth)
        .add_property("visible", &ColorLayer::visible, &ColorLayer::setVisible)
        .add_property("segments", py::make_function(+[](ColorLayer &l){ return &l.segments(); }, py::return_internal_reference<>()))
        .def("show", &ColorLayer::show)
        .def("fill", +[](ColorLayer &l, const std::string &value) { 
            l.fill(str2color(value)); 
        })
        .def("__getitem__", +[](const ColorLayer &l, uint index){
            checkIndex(l, index);
            return color2str(l[index]);
        })
        .def("__setitem__", +[](ColorLayer &l, uint index, const std::string &value) {
            checkIndex(l, index);
            l[index] = str2color(value);
        })
        .def("__setitem__", +[](ColorLayer &l, uint index, const py::tuple &value) {
            checkIndex(l, index);
            l[index] = tuple2color(value);
        })
        .def("__len__", &ColorLayer::size)
        .def("__enter__", +[](ColorLayer &l) {
            //l.mutex_lock();
            return l.shared_from_this();
        })
        .def("__exit__", +[](ColorLayer &l, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            //l.mutex_unlock();
        })
        ;

    py::class_<Control, std::shared_ptr<Control>, boost::noncopyable>("LEDControl", py::no_init)
        .add_static_property("NOTIFY_DEFAULT", py::make_getter(Control::NOTIFY_DEFAULT))
        .add_property("background", 
            +[](const Control &ctl) {
                return color2str(ctl.getBackground());
            },
            +[](Control &ctl, const std::string &value) {
                ctl.setBackground(str2color(value));
            })
        .add_property("animation", &Control::getAnimation, &Control::setAnimation)
        .add_property("indicators", &Control::getIndicators, &Control::setIndicators)
        .def("attach_layer", &Control::attachLayer)
        .def("detach_layer", &Control::detachLayer)
        .def("show", &Control::show)
        .def("subscribe", +[](Control &control) { return notify_subscribe(control); })
        .def("subscribe_attach", +[](Control &control, NotifySubscription<Control::NotifyType> &sub, int offset) { return notify_attach(sub, control, offset); })
        .def("__enter__", +[](Control &ctl) {
            ctl.mutex_lock();
            return ctl.shared_from_this();
        })
        .def("__exit__", +[](Control &ctl, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            ctl.mutex_unlock();
        })
        ;
}

}