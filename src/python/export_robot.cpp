#include <memory>
#include <boost/python.hpp>

#include "../robot.h"
#include "../rcreceiver/rcreceiver.h"
#include "../telemetry/telemetry.h"
#include "../motor/motor.h"
#include "../motor/motorcontrol.h"
#include "../led/ledcontrol.h"
#include "../led/ledcolor.h"
#include "../kinematic/kinematic.h"


using namespace boost;
using namespace boost::python;

void python_export_robot() 
{
    register_ptr_to_python<std::shared_ptr<Robot> >();

    class_<Robot, noncopyable>("Robot")
        .add_property("rc_receiver", make_function(+[](const Robot &r){ return r.rcReceiver().get(); }, return_internal_reference<>()))
        .add_property("motor_control", make_function(+[](const Robot &r){ return r.motorControl().get(); }, return_internal_reference<>()))
        .add_property("led_control", make_function(+[](const Robot &r){ return r.ledControl().get(); }, return_internal_reference<>()))
        .add_property("telemetry", make_function(+[](const Robot &r){ return r.telemetry().get(); }, return_internal_reference<>()))
        .add_property("kinematic", make_function(+[](const Robot &r){ return r.kinematic().get(); }, return_internal_reference<>()))
        .add_property("armed", &Robot::getArmed, &Robot::setArmed)
        .def("init", &Robot::init)
        .def("cleanup", &Robot::cleanup)
        ;
}
