#include <boost/format.hpp>
#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/enum.hpp> 
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/log/trivial.hpp>

#include "util.h"
#include "../motor/motor.h"
#include "../motor/motorgimbal.h"
#include "../motor/motorcontrol.h"

using namespace std;
namespace py = boost::python;


void python_export_motor() 
{
    py::register_ptr_to_python<shared_ptr<MotorControl>>();

    py::enum_<Motor::State>("MotorState");

    py::class_<Motor, boost::noncopyable>("Motor", py::no_init)
        .add_property("index", &Motor::getIndex)
        .add_property("gimbal", py::make_function(&Motor::gimbal, py::return_internal_reference<>()))
        .add_property("duty", &Motor::getDuty, &Motor::setDuty)
        .add_property("target_rpm", &Motor::getTargetRPM, &Motor::setTargetRPM)
        .add_property("rpm", &Motor::getRPM)
        .add_property("state", &Motor::getState)
        .def("brake", &Motor::brake)
        .def("free_spin", &Motor::freeSpin)
        .def("reset_odometer", &Motor::resetOdometer)
        .def("__str__", +[](const Motor &m) { return (boost::format("<Motor (%d)>") % m.getIndex()).str(); })
        ;

    py::class_<MotorGimbal, boost::noncopyable>("MotorGimbal", py::no_init)
        .add_property("index", &MotorGimbal::getIndex)
        .add_property("enabled", &MotorGimbal::getEnabled, &MotorGimbal::setEnabled)
        .add_property("pulse_us", &MotorGimbal::getPulseUS, &MotorGimbal::setPulseUS)
        .add_property("angle", &MotorGimbal::getAngle, &MotorGimbal::setAngle)
        .add_property("angle_degrees", &MotorGimbal::getAngleDegrees, &MotorGimbal::setAngleDegrees)
        .add_property("trim_us", &MotorGimbal::getTrimUS, &MotorGimbal::setTrimUS)
        .add_property("limit_min", &MotorGimbal::getLimitMin, &MotorGimbal::setLimitMin)
        .add_property("limit_max", &MotorGimbal::getLimitMax, &MotorGimbal::setLimitMax)
        .def("set_limits", &MotorGimbal::setLimits)
        .def("__str__", +[](const MotorGimbal &m) { return (boost::format("<MotorGimbal (%d)>") % m.getIndex()).str(); })
        ;

    py::class_<MotorControl::MotorList, boost::noncopyable>("MotorList", py::no_init)
        .def("__getitem__", +[](const MotorControl::MotorList &l, uint index){
            if (index >= l.size()) {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                py::throw_error_already_set();
            }
            return l[index].get();
        }, py::return_internal_reference<>())
        .def("__len__", &MotorControl::MotorList::size)
        ;

    py::class_<MotorControl, boost::noncopyable>("MotorControl", py::no_init)
        .add_property("motors", py::make_function(&MotorControl::getMotors, py::return_internal_reference<>() ))
        .def("brake", &MotorControl::brake)
        .def("free_spin", &MotorControl::freeSpin)
        .def("resetOdometer", &MotorControl::resetOdometer)
        .def("__enter__", +[](MotorControl &ctl) {
            ctl.lock();
            return ctl.shared_from_this();
        })
        .def("__exit__", +[](MotorControl &ctl, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            ctl.unlock();
        })
        ;


}