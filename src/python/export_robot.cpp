#include <boost/shared_ptr.hpp>
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

void python_export_robot() {
    register_ptr_to_python<shared_ptr<Robot> >();

    class_<Robot, noncopyable>("Robot")
        .add_property("rc_receiver", &Robot::rcReceiver)
        .add_property("motor_control", &Robot::motorControl)
        .add_property("led_control", &Robot::ledControl)
        .add_property("telemetry", &Robot::telemetry)
        .add_property("kinematic", &Robot::kinematic)
        .add_property("armed", &Robot::getArmed, &Robot::setArmed)
        .def("init", &Robot::init)
        .def("cleanup", &Robot::cleanup)
        ;
}
