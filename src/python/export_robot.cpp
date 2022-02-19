#include <memory>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include <common/notifysubscription.h>
#include <robot.h>
#include <robotcontext.h>
#include <rc/receiver.h>
#include <telemetry/telemetry.h>
#include <motor/control.h>
#include <led/control.h>
#include <kinematic/kinematic.h>
#include <input/control.h>
#include <system/power.h>
#include <system/network.h>

#include "util.h"

namespace Robot::Python {


void export_robot() 
{
    namespace py = boost::python;
    using namespace ::Robot;

    py::class_<Robot, std::shared_ptr<Robot>, py::bases<WithNotifyDefault>, boost::noncopyable>("Robot")
        .add_property("heartbeat", &Robot::Robot::heartbeat)
        .add_property("rc_receiver", py::make_function(+[](const Robot &r){ return r.rcReceiver().get(); }, py::return_internal_reference<>()))
        .add_property("motor_control", py::make_function(+[](const Robot &r){ return r.motorControl().get(); }, py::return_internal_reference<>()))
        .add_property("led_control", py::make_function(+[](const Robot &r){ return r.ledControl().get(); }, py::return_internal_reference<>()))
        .add_property("telemetry", py::make_function(+[](const Robot &r){ return r.telemetry().get(); }, py::return_internal_reference<>()))
        .add_property("kinematic", py::make_function(+[](const Robot &r){ return r.kinematic().get(); }, py::return_internal_reference<>()))
        .add_property("input", py::make_function(+[](const Robot &r){ return r.input().get(); }, py::return_internal_reference<>()))
        .add_property("power", py::make_function(+[](const Robot &r){ return r.power().get(); }, py::return_internal_reference<>()))
        .add_property("network", py::make_function(+[](const Robot &r){ return r.network().get(); }, py::return_internal_reference<>()))
        .def("init", &Robot::init)
        .def("cleanup", &Robot::cleanup)
        ;

}

}