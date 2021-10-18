#include <typeinfo>
#include <vector>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/exception/diagnostic_information.hpp> 
#include <boost/log/trivial.hpp>

#include "util.h"
#include "../robot.h"
#include "../rcreceiver.h"
#include "../telemetry/telemetry.h"
#include "../motor/motor.h"
#include "../motor/motorcontrol.h"
#include "../led/ledcontrol.h"
#include "../led/ledcolor.h"

using namespace boost;
using namespace boost::python;

void python_export_std();
void python_export_motor();
void python_export_telemetry();
void python_export_rcreceiver();
void python_export_led();


BOOST_PYTHON_MODULE(beaglerover) {
    PyEval_InitThreads();

    register_ptr_to_python<shared_ptr<Robot> >();

    python_export_std();
    python_export_motor();
    python_export_telemetry();
    python_export_rcreceiver();
    python_export_led();

    class_<Robot, noncopyable>("Robot")
        .add_property("rc_receiver", &Robot::rcReceiver)
        .add_property("motor_control", &Robot::motorControl)
        .add_property("led_control", &Robot::ledControl)
        .add_property("telemetry", &Robot::telemetry)
        .add_property("armed", &Robot::getArmed, &Robot::setArmed)
        .def("init", &Robot::init)
        .def("cleanup", &Robot::cleanup)
        ;
}