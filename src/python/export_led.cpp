#include <memory>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/utility/string_ref.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/object.hpp>
#include <boost/python/list.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include <common/notifysubscription.h>
#include <led/control.h>
#include <led/color.h>
#include <led/colorlayer.h>
#include "util.h"

namespace py = boost::python;

using Robot::LED::Color, Robot::LED::ColorLayer, Robot::LED::ColorArray;

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

static std::string color2rgbstr(const Color &color)
{
    std::stringstream stream;
    stream << boost::format("#%+02x%+02x%+02x") % (uint32_t)color.red() % (uint32_t)color.green() % (uint32_t)color.blue();
    return stream.str();
}


boost::python::tuple colors2tuple(const ColorLayer::array_type &array) {
    boost::python::tuple obj { boost::python::handle<>(PyTuple_New(array.size())) };
    int idx = 0;
    for (auto &c : array) {
        PyTuple_SET_ITEM(obj.ptr(), idx, boost::python::incref(boost::python::object(color2rgbstr(c)).ptr()));
        idx++;
    }
    return obj;
}


static inline void checkIndex(const ColorLayer &l, uint index) 
{
    if (index >= l.size()) {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        py::throw_error_already_set();
    }
}

static inline void checkIndex(const ColorLayer::Segment &l, uint index) 
{
    if (index >= l.size()) {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        py::throw_error_already_set();
    }
}

static inline void checkInternal(const ColorLayer &l) 
{
    if (l.internal()) {
        PyErr_SetString(PyExc_AttributeError, "Invalid operation on internal layer");
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

    py::enum_<Color::Correction>("LEDColorCorrection")
        .value("TypicalSMD5050", Color::Correction::TypicalSMD5050)
        .value("TypicalLEDStrip", Color::Correction::TypicalLEDStrip)
        .value("Typical8mmPixel", Color::Correction::Typical8mmPixel)
        .value("TypicalPixelString", Color::Correction::TypicalPixelString)
        .value("UncorrectedColor", Color::Correction::UncorrectedColor)
        ;


   py::class_<ColorLayer::Segment, boost::noncopyable>("LEDSegment", py::no_init)
        .add_property("name", +[](const ColorLayer::Segment &s) { return s.name(); })
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

   py::class_<ColorLayer, std::shared_ptr<ColorLayer>, boost::noncopyable>("LEDColorLayer", py::init<const std::string&, uint>())
        .add_property("name", +[](const ColorLayer &l) { return l.name(); })
        .add_property("internal", &ColorLayer::internal)
        .add_property("depth", &ColorLayer::depth)
        .add_property("visible", &ColorLayer::visible, &ColorLayer::setVisible)
        .add_property("segments", py::make_function(+[](ColorLayer &l){ return &l.segments(); }, py::return_internal_reference<>()))
        .def("update", &ColorLayer::update)
        .def("fill", +[](ColorLayer &l, const std::string &value) { 
            checkInternal(l);
            l.fill(str2color(value)); 
        })
        .def("__getitem__", +[](const ColorLayer &l, uint index){
            checkIndex(l, index);
            return color2str(l[index]);
        })
        .def("__setitem__", +[](ColorLayer &l, uint index, const std::string &value) {
            checkInternal(l);
            checkIndex(l, index);
            l[index] = str2color(value);
        })
        .def("__setitem__", +[](ColorLayer &l, uint index, const py::tuple &value) {
            checkInternal(l);
            checkIndex(l, index);
            l[index] = tuple2color(value);
        })
        .def("__len__", &ColorLayer::size)
        .def("__enter__", +[](ColorLayer &l) {
            l.mutex_lock();
            return l.shared_from_this();
        })
        .def("__exit__", +[](ColorLayer &l, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            l.mutex_unlock();
        })
        ;


    py::class_<Control::LayerList>("LEDColorLayerList", py::no_init)
        .def("__iter__", py::iterator<Control::LayerList>())
        .def("find", +[](const Control::LayerList &self, const std::string &name) {
            auto res = std::find_if(self.begin(), self.end(), [&name](const auto &l) {
                return l->name() == name;
            });
            return res != self.end() ? *res : nullptr;
        })
        ;

    py::class_<Control, std::shared_ptr<Control>, boost::noncopyable>("LEDControl", py::no_init)
        .add_static_property("NOTIFY_DEFAULT", py::make_getter(Control::NOTIFY_DEFAULT))
        .add_static_property("NOTIFY_UPDATE", py::make_getter(Control::NOTIFY_UPDATE))
        .add_property("brightness", &Control::getBrightness, &Control::setBrightness)
        .add_property("color_correction", &Control::getColorCorrection, &Control::setColorCorrection)
        .add_property("background", 
            +[](const Control &self) {
                return color2rgbstr(self.getBackground());
            },
            +[](Control &self, const std::string &value) {
                self.setBackground(str2color(value));
            })
        .add_property("animation", &Control::getAnimation, &Control::setAnimation)
        .add_property("indicators", &Control::getIndicators, &Control::setIndicators)
        .add_property("pixels", +[](Control &self) { return colors2tuple(self.pixels()); })
        .add_property("layers", &Control::layers)
        .def("attach_layer", &Control::attachLayer)
        .def("detach_layer", &Control::detachLayer)
        .def("update", &Control::update)
        .def("subscribe", +[](Control &self) { return notify_subscribe(self); })
        .def("subscribe", +[](Control &self, std::shared_ptr<NotifySubscription<Control::NotifyType>> sub, int offset) { notify_attach(*sub, self, offset); return sub; })
        .def("__enter__", +[](Control &self) {
            self.mutex_lock();
            return self.shared_from_this();
        })
        .def("__exit__", +[](Control &self, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            self.mutex_unlock();
        })
        ;
}

}