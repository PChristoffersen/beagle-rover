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
#include "errors.h"

namespace py = boost::python;

using Robot::LED::Color, Robot::LED::Control, Robot::LED::ColorLayer, Robot::LED::ColorArray;

namespace Robot::Python {


template<typename T>
boost::python::tuple colors2tuple(const T &array) {
    boost::python::tuple obj { boost::python::handle<>(PyTuple_New(array.size())) };
    for (typename T::size_type i=0; i<array.size(); i++) {
        PyTuple_SET_ITEM(obj.ptr(), i, boost::python::incref(boost::python::object(array[i].toString()).ptr()));
    }
    return obj;
}



void export_led() 
{
    using Robot::LED::AnimationMode, Robot::LED::IndicatorMode;
    using Robot::LED::color_array_type;

    py::enum_<AnimationMode>("LEDAnimation")
        .value("NONE", AnimationMode::NONE)
        .value("HEADLIGHTS", AnimationMode::HEADLIGHTS)
        .value("CONSTRUCTION", AnimationMode::CONSTRUCTION)
        .value("POLICE", AnimationMode::POLICE)
        .value("AMBULANCE", AnimationMode::AMBULANCE)
        .value("RUNNING_LIGHT", AnimationMode::RUNNING_LIGHT)
        .value("KNIGHT_RIDER", AnimationMode::KNIGHT_RIDER)
        .value("RAINBOW", AnimationMode::RAINBOW)
        .value("RAINBOW_WAVE", AnimationMode::RAINBOW_WAVE)
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


    // -----------------------------------------------------
    // Base color array and segments
    // -----------------------------------------------------
    py::class_<color_array_type, boost::noncopyable>("LEDColorArray", py::no_init)
        .add_property("segments", py::make_function(+[](color_array_type &self){ return &self.segments(); }, py::return_internal_reference<>()))
        .def("values", +[](const color_array_type &self) { 
            color_array_type::guard lock(self.mutex());
            return colors2tuple(self); 
        })
        .def("fill", +[](color_array_type &self, const std::string &value) { self.fill(value); })
        .def("__getitem__", +[](const color_array_type &self, uint index){
            if (index >= self.size()) {
                BOOST_THROW_EXCEPTION(std::out_of_range("Index out of range"));
            }
            return self[index].toString();
        })
        .def("__setitem__", +[](color_array_type &self, uint index, const std::string &value) {
            if (index >= self.size()) {
                BOOST_THROW_EXCEPTION(std::out_of_range("Index out of range"));
            }
            self[index] = value;
        })
        .def("__len__", &color_array_type::size)
        .def("__enter__", +[](color_array_type &self) {
            self.mutex_lock();
        })
        .def("__exit__", +[](color_array_type &self, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            self.mutex_unlock();
        })
        ;

    py::class_<color_array_type::SegmentList, boost::noncopyable>("LEDColorSegmentList", py::no_init)
        .def("keys", +[](const color_array_type::SegmentList &self) { 
            py::list res;
            for (const auto &s : self) {
                res.append(s.name().data());
            }
            return res;
        })
        .def("__getitem__", +[](color_array_type::SegmentList &array, uint index) {
            if (index >= array.size()) {
                BOOST_THROW_EXCEPTION(std::out_of_range("Index out of range"));
            }
            return &array[index];
        }, py::return_internal_reference<>())
        .def("__getitem__", +[](color_array_type::SegmentList &array, const std::string &name) {
            return &array.find(name);
        }, py::return_internal_reference<>())
        .def("__len__", &color_array_type::SegmentList::size)
        ;

    py::class_<color_array_type::Segment, boost::noncopyable>("LEDColorSegment", py::no_init)
        .add_property("name", +[](const ColorLayer::Segment &s) { return s.name().data(); })
        .add_property("offset", &color_array_type::Segment::offset)
        .add_property("size", &color_array_type::Segment::size)
        .def("fill", +[](color_array_type::Segment &self, const std::string &value) { self.fill(value); })
        .def("values", +[](const color_array_type::Segment &self) { 
            color_array_type::guard lock(self.parent()->mutex());
            return colors2tuple(self); 
        })
        .def("__getitem__", +[](const ColorLayer::Segment &segment, uint index){
            if (index >= segment.size()) {
                BOOST_THROW_EXCEPTION(std::out_of_range("Index out of range"));
            }
            return segment[index].toStringRGBA();
        })
        .def("__setitem__", +[](ColorLayer::Segment &segment, uint index, const std::string &value) {
            if (index >= segment.size()) {
                BOOST_THROW_EXCEPTION(std::out_of_range("Index out of range"));
            }
            segment[index] = value;
        })
        .def("__len__", &ColorLayer::Segment::size)
        ;


    // -----------------------------------------------------
    // ColorLayer
    // -----------------------------------------------------
    py::class_<ColorLayer, std::shared_ptr<ColorLayer>, py::bases<color_array_type>, boost::noncopyable>("LEDColorLayer", py::init<const std::string&, uint>())
        .add_property("name", +[](const ColorLayer &l) { return l.name(); })
        .add_property("internal", &ColorLayer::internal)
        .add_property("depth", &ColorLayer::depth)
        .add_property("visible", &ColorLayer::visible, &ColorLayer::setVisible)
        .add_property("segments", py::make_function(+[](ColorLayer &l){ return &l.segments(); }, py::return_internal_reference<>()))
        .def("update", &ColorLayer::update)
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


    // -----------------------------------------------------
    // Control
    // -----------------------------------------------------
    py::class_<Control, std::shared_ptr<Control>, py::bases<WithNotifyDefault>, boost::noncopyable>("LEDControl", py::no_init)
        .add_static_property("NOTIFY_UPDATE", py::make_getter(Control::NOTIFY_UPDATE))
        .add_property("brightness", &Control::getBrightness, &Control::setBrightness)
        .add_property("color_correction", &Control::getColorCorrection, &Control::setColorCorrection)
        .add_property("background", 
            +[](const Control &self) {
                return static_cast<std::string>(self.getBackground());
            },
            +[](Control &self, const std::string &value) {
                self.setBackground(value);
            })
        .add_property("animation", &Control::getAnimation, &Control::setAnimation)
        .add_property("indicators", &Control::getIndicators, &Control::setIndicators)
        .add_property("output", py::make_function(&Control::output, py::return_internal_reference<>()))
        .add_property("layers", &Control::layers)
        .def("attach_layer", &Control::attachLayer)
        .def("detach_layer", &Control::detachLayer)
        .def("update", &Control::update)
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