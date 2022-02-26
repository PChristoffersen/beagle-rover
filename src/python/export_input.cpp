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

    py::class_<SoftwareInterface, std::shared_ptr<SoftwareInterface>, py::bases<WithNotifyDefault>, boost::noncopyable>("InputInterface", py::no_init)
        .add_property("steering", &SoftwareInterface::getSteering, &SoftwareInterface::setSteering)
        .add_property("throttle", &SoftwareInterface::getThrottle, &SoftwareInterface::setThrottle)
        .add_property("aux_x", &SoftwareInterface::getAuxX, &SoftwareInterface::setAuxX)
        .add_property("aux_y", &SoftwareInterface::getAuxY, &SoftwareInterface::setAuxY)
        .def("set_axis", +[](SoftwareInterface &iface, float s, float t) { iface.setAxis(s,t); })
        .def("set_axis", &SoftwareInterface::setAxis)
        ;

    py::class_<Control, std::shared_ptr<Control>, py::bases<WithNotifyDefault>, boost::noncopyable>("InputControl", py::no_init)
        .add_property("axis_source", &Control::getAxisSource, &Control::setAxisSource)
        .add_property("kinematic_source", &Control::getKinematicSource, &Control::setKinematicSource)
        .add_property("led_source", &Control::getLedSource, &Control::setLedSource)
        .add_property("manual", &Control::manual)
        .add_property("web", &Control::web)
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