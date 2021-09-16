#include <boost/python.hpp>

#include "robot.h"
#include "motor/motorcontrol.h"
#include "led/ledcontrol.h"
#include "telemetry/telemetry.h"

BOOST_PYTHON_MODULE(beaglerover) {
    using namespace boost;
    using namespace boost::python;

    class_<MotorControl, noncopyable>("MotorControl", no_init)
        ;


    class_<LEDControl, noncopyable>("LEDControl", no_init)
        ;


    class_<Telemetry, noncopyable>("Telemetry", no_init)
        ;


    register_ptr_to_python<shared_ptr<MotorControl> >();
    register_ptr_to_python<shared_ptr<LEDControl> >();
    register_ptr_to_python<shared_ptr<Telemetry> >();


    class_<Robot, noncopyable>("Robot")
        .add_property("motor_control", &Robot::motorControl)
        .add_property("led_control", &Robot::ledControl)
        .add_property("telemetry", &Robot::telemetry)
        .def("init", &Robot::init)
        .def("cleanup", &Robot::cleanup)
        ;
}