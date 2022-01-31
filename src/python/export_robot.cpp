#include <memory>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include <robot.h>
#include <rc/receiver.h>
#include <telemetry/telemetry.h>
#include <motor/control.h>
#include <led/control.h>
#include <kinematic/kinematic.h>
#include <input/control.h>


namespace py = boost::python;

namespace Robot::Python {


class RobotDummy { 
};

void export_robot() 
{
    using namespace ::Robot;

    py::class_<Robot, std::shared_ptr<Robot>, boost::noncopyable>("Robot")
        .add_property("heartbeat", &Robot::Robot::heartbeat)
        .add_property("rc_receiver", py::make_function(+[](const Robot &r){ 
            #if ROBOT_HAVE_RC
            return r.rcReceiver().get(); 
            #else
            return (RobotDummy*)nullptr;
            #endif
        }, py::return_internal_reference<>()))
        .add_property("motor_control", py::make_function(+[](const Robot &r){ return r.motorControl().get(); }, py::return_internal_reference<>()))
        .add_property("led_control", py::make_function(+[](const Robot &r){ return r.ledControl().get(); }, py::return_internal_reference<>()))
        .add_property("telemetry", py::make_function(+[](const Robot &r){ return r.telemetry().get(); }, py::return_internal_reference<>()))
        .add_property("kinematic", py::make_function(+[](const Robot &r){ return r.kinematic().get(); }, py::return_internal_reference<>()))
        .add_property("input", py::make_function(+[](const Robot &r){ return r.input().get(); }, py::return_internal_reference<>()))
        .def("init", &Robot::init)
        .def("cleanup", &Robot::cleanup)
        ;
}

};