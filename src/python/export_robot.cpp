#include <memory>
#include <boost/python.hpp>

#include "../robot.h"
#include "../rc/rcreceiver.h"
#include "../telemetry/telemetry.h"
#include "../motor/motor.h"
#include "../motor/motorcontrol.h"
#include "../led/ledcontrol.h"
#include "../led/ledcolor.h"
#include "../kinematic/kinematic.h"


using namespace std;
namespace py = boost::python;

void python_export_robot() 
{
    py::class_<Robot::Robot, shared_ptr<Robot::Robot>, boost::noncopyable>("Robot")
        .add_property("rc_receiver", py::make_function(+[](const Robot::Robot &r){ return r.rcReceiver().get(); }, py::return_internal_reference<>()))
        .add_property("motor_control", py::make_function(+[](const Robot::Robot &r){ return r.motorControl().get(); }, py::return_internal_reference<>()))
        .add_property("led_control", py::make_function(+[](const Robot::Robot &r){ return r.ledControl().get(); }, py::return_internal_reference<>()))
        .add_property("telemetry", py::make_function(+[](const Robot::Robot &r){ return r.telemetry().get(); }, py::return_internal_reference<>()))
        .add_property("kinematic", py::make_function(+[](const Robot::Robot &r){ return r.kinematic().get(); }, py::return_internal_reference<>()))
        .add_property("armed", &Robot::Robot::getArmed, &Robot::Robot::setArmed)
        .def("init", &Robot::Robot::init)
        .def("cleanup", &Robot::Robot::cleanup)
        ;
}
