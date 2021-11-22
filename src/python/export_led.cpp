#include <memory>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/tuple.hpp>
#include <boost/python/object.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "../led/ledcontrol.h"
#include "../led/ledcolor.h"
#include "util.h"

using namespace std;
using namespace Robot::LED;
namespace py = boost::python;



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


void checkLayerIndex(const ColorLayer &l, uint index) 
{
    if (index >= l.size()) {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        py::throw_error_already_set();
    }
}



void python_export_led() 
{
    /*
    iterable_converter()
        .from_python<ColorList>()
        ;
    */

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

   py::class_<ColorLayer, shared_ptr<ColorLayer>, boost::noncopyable>("LEDColorLayer", py::init<int>())
        .add_property("depth", &ColorLayer::depth)
        .add_property("visible", &ColorLayer::visible, &ColorLayer::setVisible)
        .def("detach", &ColorLayer::detach)
        .def("show", &ColorLayer::show)
        .def("fill", +[](ColorLayer &l, std::uint32_t value) { 
            l.fill(Color { value }); 
        })
        .def("fill", +[](ColorLayer &l, const py::tuple &value) { 
            l.fill(tuple2color(value)); 
        })
        .def("__getitem__", +[](const ColorLayer &l, uint index){
            checkLayerIndex(l, index);
            const auto &color = l[index];
            return py::make_tuple(color.red(), color.green(), color.blue(), color.alpha());
        })
        .def("__setitem__", +[](ColorLayer &l, uint index, std::uint32_t value) {
            checkLayerIndex(l, index);
            l[index] = value;
        })
        .def("__setitem__", +[](ColorLayer &l, uint index, const py::tuple &value) {
            checkLayerIndex(l, index);
            l[index] = tuple2color(value);
        })
        .def("__len__", &ColorLayer::size)
        .def("__enter__", +[](ColorLayer &l) {
            l.lock();
            return l.shared_from_this();
        })
        .def("__exit__", +[](ColorLayer &l, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            l.unlock();
        })
        ;

    py::class_<Control, shared_ptr<Control>, boost::noncopyable>("LEDControl", py::no_init)
        .add_property("background", 
            +[](const Control &ctl) {
                const auto color = ctl.getBackground();
                return py::make_tuple(color.red(), color.green(), color.blue(), color.alpha());
            },
            +[](Control &ctl, py::tuple &value) {
                ctl.setBackground(tuple2color(value));
            })
        .add_property("animation", &Control::getAnimation, &Control::setAnimation)
        .add_property("indicators", &Control::getIndicators, &Control::setIndicators)
        .def("attach_layer", &Control::attachLayer)
        .def("detach_layer", &Control::detachLayer)
        .def("show", &Control::show)
        .def("__enter__", +[](Control &ctl) {
            ctl.lock();
            return ctl.shared_from_this();
        })
        .def("__exit__", +[](Control &ctl, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            ctl.unlock();
        })
        ;
}