#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#include <boost/python/enum.hpp> 
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS


#include <common/notifysubscription.h>
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

    py::class_<Motor, py::bases<WithNotifyInt, WithMutexStd>, boost::noncopyable>("Motor", py::no_init)
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
        .def("__str__", +[](const Motor &m) { return (boost::format("<Motor (%d)>") % m.getIndex()).str(); })
        ;

    py::class_<Servo, py::bases<WithNotifyInt, WithMutexStd>, boost::noncopyable>("Servo", py::no_init)
        .add_static_property("NOTIFY_TELEMETRY", py::make_getter(Servo::NOTIFY_TELEMETRY))
        .add_property("index", &Servo::getIndex)
        .add_property("enabled", &Servo::getEnabled, &Servo::setEnabled)
        .add_property("pulse_us", +[](const Servo &self) { return self.getValue().asServoPulse(); }, +[](Servo &self, uint32_t value) { self.setValue(Value::fromMicroSeconds(value)); })
        .add_property("angle_radians", +[](const Servo &self) { return self.getValue().asAngle(); }, +[](Servo &self, float value) { self.setValue(Value::fromAngle(value)); })
        .add_property("angle", +[](const Servo &self) { return self.getValue().asAngleDegrees(); }, +[](Servo &self, float value) { self.setValue(Value::fromAngleDegrees(value)); })
        .add_property("limit_min", +[](const Servo &self) { return self.getLimitMin().asAngleDegrees(); }, +[](Servo &self, float value) { self.setLimitMin(Value::fromAngleDegrees(value)); })
        .add_property("limit_max", +[](const Servo &self) { return self.getLimitMax().asAngleDegrees(); }, +[](Servo &self, float value) { self.setLimitMax(Value::fromAngleDegrees(value)); })
        .def("set_limits", +[](Servo &self, float min, float max) { self.setLimits(Value::fromAngle(min), Value::fromAngle(max)); })
        .def("__str__", +[](const Servo &m) { return (boost::format("<Servo (%d)>") % m.getIndex()).str(); })
        ;

    py::class_<MotorList, boost::noncopyable>("MotorList", py::no_init)
        .def("__getitem__", +[](const MotorList &l, uint index){
            if (index >= l.size()) {
                BOOST_THROW_EXCEPTION(std::out_of_range("Index out of range"));
            }
            return l[index].get();
        }, py::return_internal_reference<>())
        .def("__len__", &MotorList::size)
        ;

    py::class_<Control, py::bases<WithNotifyInt, WithMutexStd>, std::shared_ptr<Control>, boost::noncopyable>("MotorControl", py::no_init)
        .add_property("motors", py::make_function(&Control::getMotors, py::return_internal_reference<>() ))
        .def("__str__", +[](const Control &self) { return "<MotorControl>"; })
        ;

}

}