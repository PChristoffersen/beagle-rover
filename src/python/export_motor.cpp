#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/enum.hpp> 
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "util.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"

using namespace boost;
using namespace boost::python;


void python_export_motor() {
    register_ptr_to_python<shared_ptr<Motor> >();
    register_ptr_to_python<shared_ptr<MotorGimbal> >();
    register_ptr_to_python<shared_ptr<MotorControl> >();

    enum_<Motor::State>("MotorState");

    class_<Motor, noncopyable>("Motor", no_init)
        .add_property("index", &Motor::getIndex)
        .add_property("gimbal", &Motor::gimbal)
        .add_property("duty", &Motor::getDuty, &Motor::setDuty)
        .add_property("target_rpm", &Motor::getTargetRPM, &Motor::setTargetRPM)
        .add_property("rpm", &Motor::getRPM)
        .add_property("state", &Motor::getState)
        .def("brake", &Motor::brake)
        .def("free_spin", &Motor::freeSpin)
        ;
    class_<MotorGimbal, noncopyable>("MotorGimbal", no_init)
        .add_property("enabled", &MotorGimbal::getEnabled, &MotorGimbal::setEnabled)
        .add_property("pulse_us", &MotorGimbal::getPulseUS, &MotorGimbal::setPulseUS)
        ;
    class_<MotorControl::MotorList>("MotorList")
        .def(vector_indexing_suite<MotorControl::MotorList, true>() )
        ;
    class_<MotorControl, noncopyable>("MotorControl", no_init)
        .add_property("motors", &MotorControl::getMotors)
        .def("brake", &MotorControl::brake)
        .def("free_spin", &MotorControl::freeSpin)
        ;


}