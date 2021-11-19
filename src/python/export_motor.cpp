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
using namespace Robot::Motor;
namespace py = boost::python;


void python_export_motor() 
{
    //py::register_ptr_to_python<shared_ptr<MotorControl>>();

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

    py::class_<Gimbal, boost::noncopyable>("Gimbal", py::no_init)
        .add_property("index", &Gimbal::getIndex)
        .add_property("enabled", &Gimbal::getEnabled, &Gimbal::setEnabled)
        .add_property("pulse_us", &Gimbal::getPulseUS, &Gimbal::setPulseUS)
        .add_property("angle", &Gimbal::getAngle, &Gimbal::setAngle)
        .add_property("angle_degrees", &Gimbal::getAngleDegrees, &Gimbal::setAngleDegrees)
        .add_property("trim_us", &Gimbal::getTrimUS, &Gimbal::setTrimUS)
        .add_property("limit_min", &Gimbal::getLimitMin, &Gimbal::setLimitMin)
        .add_property("limit_max", &Gimbal::getLimitMax, &Gimbal::setLimitMax)
        .def("set_limits", &Gimbal::setLimits)
        .def("__str__", +[](const Gimbal &m) { return (boost::format("<Gimbal (%d)>") % m.getIndex()).str(); })
        ;

    py::class_<Control::MotorList, boost::noncopyable>("MotorList", py::no_init)
        .def("__getitem__", +[](const Control::MotorList &l, uint index){
            if (index >= l.size()) {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                py::throw_error_already_set();
            }
            return l[index].get();
        }, py::return_internal_reference<>())
        .def("__len__", &Control::MotorList::size)
        ;

    py::class_<Control, shared_ptr<Control>, boost::noncopyable>("MotorControl", py::no_init)
        .add_property("motors", py::make_function(&Control::getMotors, py::return_internal_reference<>() ))
        .def("brake", &Control::brake)
        .def("free_spin", &Control::freeSpin)
        .def("resetOdometer", &Control::resetOdometer)
        .def("__enter__", +[](Control &ctl) {
            ctl.lock();
            return ctl.shared_from_this();
        })
        .def("__exit__", +[](Control &ctl, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            ctl.unlock();
        })
        ;


}