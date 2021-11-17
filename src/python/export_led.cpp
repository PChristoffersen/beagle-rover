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
namespace py = boost::python;




static LEDColor tuple2color(const py::tuple & value) 
{
    if (py::len(value)==3) {
        return LEDColor { 
            py::extract<std::uint8_t>(value[0]), 
            py::extract<std::uint8_t>(value[1]), 
            py::extract<std::uint8_t>(value[2]) 
        };
    }
    else if (py::len(value)==4) {
        return LEDColor { 
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


void checkLayerIndex(const LEDColorLayer &l, uint index) 
{
    if (index >= l.size()) {
        PyErr_SetString(PyExc_IndexError, "Index out of range");
        py::throw_error_already_set();
    }
}



void python_export_led() 
{
    py::register_ptr_to_python<shared_ptr<LEDControl> >();
    //py::register_ptr_to_python<shared_ptr<LEDColorLayer> >();

    /*
    iterable_converter()
        .from_python<LEDColorList>()
        ;
    */

    /*
    class_<LEDColor>("LEDColor", init<uint8_t, uint8_t, uint8_t>())
        .add_property("red", &LEDColor::getRed)
        .add_property("green", &LEDColor::getGreen)
        .add_property("blue", &LEDColor::getBlue)
        ;
    */
   py::class_<LEDColorLayer, shared_ptr<LEDColorLayer>, boost::noncopyable>("LEDColorLayer", py::init<int>())
        .add_property("depth", &LEDColorLayer::depth)
        .add_property("visible", &LEDColorLayer::visible, &LEDColorLayer::setVisible)
        .def("fill", +[](LEDColorLayer &l, std::uint32_t value) { 
            l.fill(LEDColor { value }); 
        })
        .def("fill", +[](LEDColorLayer &l, const py::tuple &value) { 
            l.fill(tuple2color(value)); 
        })
        .def("__getitem__", +[](const LEDColorLayer &l, uint index){
            checkLayerIndex(l, index);
            const auto &color = l[index];
            return py::make_tuple(color.red(), color.green(), color.blue(), color.alpha());
        })
        .def("__setitem__", +[](LEDColorLayer &l, uint index, std::uint32_t value) {
            checkLayerIndex(l, index);
            l[index] = value;
        })
        .def("__setitem__", +[](LEDColorLayer &l, uint index, const py::tuple &value) {
            checkLayerIndex(l, index);
            l[index] = tuple2color(value);
        })
        .def("__len__", &LEDColorLayer::size)
        ;

    py::class_<LEDControl, boost::noncopyable>("LEDControl", py::no_init)
        .add_property("background", 
            +[](const LEDControl &ctl) {
                const auto color = ctl.getBackground();
                return py::make_tuple(color.red(), color.green(), color.blue(), color.alpha());
            },
            +[](LEDControl &ctl, py::tuple &value) {
                ctl.setBackground(tuple2color(value));
            })
        .def("add_layer", &LEDControl::addLayer)
        .def("remove_layer", &LEDControl::removeLayer)
        .def("show", &LEDControl::show)
        .def("__enter__", +[](LEDControl &ctl) {
            ctl.lock();
            return ctl.shared_from_this();
        })
        .def("__exit__", +[](LEDControl &ctl, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            ctl.unlock();
        })
        ;
}