#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS


#include <common/notifysubscription.h>
#include <input/control.h>
#include <input/softwareinterface.h>
#include "util.h"

namespace py = boost::python;


namespace Robot::Python {


void export_input() 
{
    using Robot::Input::Control, Robot::Input::InputSource, Robot::Input::SoftwareInterface;

    py::enum_<InputSource>("InputSource")
        .value("MANUAL", InputSource::MANUAL)
        .value("RC", InputSource::RC)
        .value("WEB", InputSource::WEB)
        .value("CONTROLLER", InputSource::CONTROLLER)
        ;

    py::class_<SoftwareInterface, boost::noncopyable>("InputInterface", py::no_init)
        .def("steer", +[](SoftwareInterface &iface, float s, float t) { iface.steer(s,t); })
        .def("steer", &SoftwareInterface::steer)
        ;

    py::class_<Control, std::shared_ptr<Control>, py::bases<WithNotifyDefault>, boost::noncopyable>("InputControl", py::no_init)
        .add_property("source", &Control::getSource, &Control::setSource)
        .add_property("kinematic_source", &Control::getKinematicSource, &Control::setKinematicSource)
        .add_property("led_source", &Control::getLedSource, &Control::setLedSource)
        .add_property("manual", py::make_function(&Control::manual, py::return_internal_reference<>() ))
        .add_property("web", py::make_function(&Control::web, py::return_internal_reference<>() ))
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