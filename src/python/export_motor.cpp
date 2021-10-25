#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/enum.hpp> 
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/log/trivial.hpp>

#include "util.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"

using namespace boost;
using namespace boost::python;


void python_export_motor() {
    //register_ptr_to_python<std::shared_ptr<MotorControl>>();

    enum_<Motor::State>("MotorState");

    class_<Motor, noncopyable>("Motor", no_init)
        .add_property("index", &Motor::getIndex)
        .add_property("gimbal",  make_function(&Motor::gimbal, return_internal_reference<>()))
        .add_property("duty", &Motor::getDuty, &Motor::setDuty)
        .add_property("target_rpm", &Motor::getTargetRPM, &Motor::setTargetRPM)
        .add_property("rpm", &Motor::getRPM)
        .add_property("state", &Motor::getState)
        .def("brake", &Motor::brake)
        .def("free_spin", &Motor::freeSpin)
        .def("resetOdometer", &Motor::resetOdometer)
        ;
    class_<MotorGimbal, noncopyable>("MotorGimbal", no_init)
        .add_property("index", &MotorGimbal::getIndex)
        .add_property("enabled", &MotorGimbal::getEnabled, &MotorGimbal::setEnabled)
        .add_property("pulse_us", &MotorGimbal::getPulseUS, &MotorGimbal::setPulseUS)
        .add_property("angle", &MotorGimbal::getAngle, &MotorGimbal::setAngle)
        .add_property("trim_us", &MotorGimbal::getTrimUS, &MotorGimbal::setTrimUS)
        .add_property("limit_min", &MotorGimbal::getLimitMin, &MotorGimbal::setLimitMin)
        .add_property("limit_max", &MotorGimbal::getLimitMax, &MotorGimbal::setLimitMax)
        .def("set_limits", &MotorGimbal::setLimits)
        ;
    class_<MotorControl::MotorList, noncopyable>("MotorList")
        .def("__getitem__", +[](const MotorControl::MotorList &l, uint index){
            if (index >= l.size()) {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                throw_error_already_set();
            }
            return l[index].get();
        }, return_internal_reference<>())
        .def("__len__", +[](const MotorControl::MotorList &l){ return l.size(); })
        ;
    class_<MotorControl, noncopyable>("MotorControl", no_init)
        .add_property("motors", make_function(&MotorControl::getMotors, return_internal_reference<>() ))
        .def("brake", &MotorControl::brake)
        .def("free_spin", &MotorControl::freeSpin)
        .def("resetOdometer", &MotorControl::resetOdometer)
        ;


}