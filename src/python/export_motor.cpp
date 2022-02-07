#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#include <boost/python/enum.hpp> 
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS


#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>
#include "util.h"

namespace py = boost::python;

namespace Robot::Python {


void export_motor() 
{
    using Robot::Motor::Control, Robot::Motor::MotorList, Robot::Motor::Motor, Robot::Motor::Servo, Robot::Motor::Value;

    py::enum_<Motor::Mode>("MotorMode")
        .value("DUTY", Motor::Mode::DUTY)
        .value("RPM", Motor::Mode::RPM)
        .value("FREE_SPIN", Motor::Mode::FREE_SPIN)
        .value("BRAKE", Motor::Mode::BRAKE)
        ;

    py::class_<Motor, boost::noncopyable>("Motor", py::no_init)
        .add_property("index", &Motor::getIndex)
        .add_property("enabled", &Motor::getEnabled, &Motor::setEnabled)
        .add_property("duty", &Motor::getDuty, &Motor::setDuty)
        .add_property("target_rpm", &Motor::getTargetRPM, &Motor::setTargetRPM)
        .add_property("rpm", &Motor::getRPM)
        .add_property("mode", &Motor::getMode)
        .add_property("odometer", &Motor::getOdometer)
        .add_property("encoder", &Motor::getEncoderValue)
        .add_property("servo", py::make_function(&Motor::servo, py::return_internal_reference<>()))
        .def("brake", &Motor::brake)
        .def("free_spin", &Motor::freeSpin)
        .def("reset_odometer", &Motor::resetOdometer)
        .def("subscribe", +[](Motor &motor, py::object &func) { return notify_subscribe<Motor>(motor, func); })
        .def("__str__", +[](const Motor &m) { return (boost::format("<Motor (%d)>") % m.getIndex()).str(); })
        ;

    py::class_<Servo, boost::noncopyable>("Servo", py::no_init)
        .add_property("index", &Servo::getIndex)
        .add_property("enabled", &Servo::getEnabled, &Servo::setEnabled)
        .add_property("pulse_us", +[](const Servo &servo) { return servo.getValue().asServoPulse(); }, +[](Servo &servo, uint32_t value) { servo.setValue(Value::fromMicroSeconds(value)); })
        .add_property("angle", +[](const Servo &servo) { return servo.getValue().asAngle(); }, +[](Servo &servo, float value) { servo.setValue(Value::fromAngle(value)); })
        .add_property("angle_degrees", +[](const Servo &servo) { return servo.getValue().asAngleDegrees(); }, +[](Servo &servo, float value) { servo.setValue(Value::fromAngleDegrees(value)); })
        .add_property("limit_min", &Servo::getLimitMin, &Servo::setLimitMin)
        .add_property("limit_max", &Servo::getLimitMax, &Servo::setLimitMax)
        .def("set_limits", &Servo::setLimits)
        .def("subscribe", +[](Servo &servo, py::object &func) { return notify_subscribe<Servo>(servo, func); })
        .def("__str__", +[](const Servo &m) { return (boost::format("<Servo (%d)>") % m.getIndex()).str(); })
        ;

    py::class_<MotorList, boost::noncopyable>("MotorList", py::no_init)
        .def("__getitem__", +[](const MotorList &l, uint index){
            if (index >= l.size()) {
                PyErr_SetString(PyExc_IndexError, "Index out of range");
                py::throw_error_already_set();
            }
            return l[index].get();
        }, py::return_internal_reference<>())
        .def("__len__", &MotorList::size)
        ;

    py::class_<Control, std::shared_ptr<Control>, boost::noncopyable>("MotorControl", py::no_init)
        .add_property("motors", py::make_function(&Control::getMotors, py::return_internal_reference<>() ))
        .add_property("odometer", &Control::getOdometer)
        .def("brake", &Control::brake)
        .def("free_spin", &Control::freeSpin)
        .def("reset_odometer", &Control::resetOdometer)
        .def("__enter__", +[](Control &ctl) {
            ctl.lock();
            return ctl.shared_from_this();
        })
        .def("__exit__", +[](Control &ctl, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            ctl.unlock();
        })
        .def("__str__", +[](const Control &ctl) { return "<MotorControl>"; })
        ;

}

};