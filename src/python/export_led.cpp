#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "../led/ledcontrol.h"
#include "../led/ledcolor.h"
#include "util.h"

using namespace boost;
using namespace boost::python;


void python_export_led() {
    register_ptr_to_python<shared_ptr<LEDControl> >();

    iterable_converter()
        .from_python<LEDColorList>()
        ;
    class_<LEDColor>("LEDColor", init<uint8_t, uint8_t, uint8_t>())
        .add_property("red", &LEDColor::getRed)
        .add_property("green", &LEDColor::getGreen)
        .add_property("blue", &LEDColor::getBlue)
        ;
    class_<LEDColorList>("LEDColorList")
        .def(vector_indexing_suite<LEDColorList, true>() )
        ;
    class_<LEDControl, noncopyable>("LEDControl", no_init)
        .def("update_pixels", &LEDControl::updatePixels)
        ;
}