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
#include "subscription.h"

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
        .add_static_property("NOTIFY_DEFAULT", py::make_getter(Motor::NOTIFY_DEFAULT))
        .add_static_property("NOTIFY_TELEMETRY", py::make_getter(Motor::NOTIFY_TELEMETRY))
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
        .def("subscribe", +[](Motor &motor) { return notify_subscribe<Motor>(motor); })
        .def("subscribe_attach", +[](Motor &motor, NotifySubscription<Motor::NotifyType> &sub, int offset) { return notify_attach<Motor>(sub, motor, offset); })
        .def("__str__", +[](const Motor &m) { return (boost::format("<Motor (%d)>") % m.getIndex()).str(); })
        ;

    py::class_<Servo, boost::noncopyable>("Servo", py::no_init)
        .add_static_property("NOTIFY_DEFAULT", py::make_getter(Servo::NOTIFY_DEFAULT))
        .add_property("index", &Servo::getIndex)
        .add_property("enabled", &Servo::getEnabled, &Servo::setEnabled)
        .add_property("pulse_us", +[](const Servo &servo) { return servo.getValue().asServoPulse(); }, +[](Servo &servo, uint32_t value) { servo.setValue(Value::fromMicroSeconds(value)); })
        .add_property("angle", +[](const Servo &servo) { return servo.getValue().asAngle(); }, +[](Servo &servo, float value) { servo.setValue(Value::fromAngle(value)); })
        .add_property("angle_degrees", +[](const Servo &servo) { return servo.getValue().asAngleDegrees(); }, +[](Servo &servo, float value) { servo.setValue(Value::fromAngleDegrees(value)); })
        .add_property("limit_min", +[](const Servo &servo) { return servo.getLimitMin().asAngle(); }, +[](Servo &servo, float value) { servo.setLimitMin(Value::fromAngle(value)); })
        .add_property("limit_max", +[](const Servo &servo) { return servo.getLimitMax().asAngle(); }, +[](Servo &servo, float value) { servo.setLimitMax(Value::fromAngle(value)); })
        .def("set_limits", +[](Servo &servo, float min, float max) { servo.setLimits(Value::fromAngle(min), Value::fromAngle(max)); })
        .def("subscribe", +[](Servo &servo) { return notify_subscribe<>(servo); })
        .def("subscribe_attach", +[](Servo &servo, NotifySubscription<Servo::NotifyType> &sub, int offset) { return notify_attach<>(sub, servo, offset); })
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
            BOOST_LOG_TRIVIAL(info) << "MotorControl __enter__";
            ctl.mutex_lock();
            return ctl.shared_from_this();
        })
        .def("__exit__", +[](Control &ctl, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            BOOST_LOG_TRIVIAL(info) << "MotorControl __exit__";
            ctl.mutex_unlock();
        })
        .def("__str__", +[](const Control &ctl) { return "<MotorControl>"; })
        ;

}

}