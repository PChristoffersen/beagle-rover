#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include <robotconfig.h>
#include <common/notifysubscription.h>
#include <system/network.h>
#include <system/power.h>
#include "util.h"

namespace py = boost::python;

namespace Robot::Python {

void export_system() 
{
    using Robot::System::Network;
    using Robot::System::Power, Robot::System::PowerSource, Robot::System::PowerSourceUnknown, Robot::System::PowerSourceBattery;

    py::class_<Network, std::shared_ptr<Network>, py::bases<WithNotifyDefault>, boost::noncopyable>("Network", py::no_init)
        ;



    py::enum_<PowerSource::Type>("PowerSourceType")
        .value("UNKNOWN", PowerSource::Type::UNKNOWN)
        .value("BATTERY", PowerSource::Type::BATTERY)
        ;

    py::class_<PowerSource, boost::noncopyable>("PowerSource", py::no_init) 
        .add_property("name", &PowerSource::name)
        .add_property("type", &PowerSource::type)
        .def("__enter__", +[](PowerSource *src) { 
            src->mutex_lock(); 
            return src;
        }, py::return_internal_reference<>())
        .def("__exit__", +[](PowerSource *src, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            src->mutex_unlock();
        })
        ;

    py::class_<PowerSourceUnknown, py::bases<PowerSource>, boost::noncopyable>("PowerSourceUnknown", py::no_init) 
        ;

    py::class_<PowerSourceBattery, py::bases<PowerSource>, boost::noncopyable>("PowerSourceBattery", py::no_init) 
        .add_property("charging", &PowerSourceBattery::charging)
        .add_property("on_battery", &PowerSourceBattery::onBattery)
        .add_property("jack_voltage", &PowerSourceBattery::jackVoltage)
        .add_property("percent", &PowerSourceBattery::percent)
        .add_property("voltage", &PowerSourceBattery::voltage)
        ;

    py::class_<Power, std::shared_ptr<Power>, py::bases<WithNotifyDefault>, boost::noncopyable>("Power", py::no_init)
        .add_property("system", py::make_function(+[](const Power &self){ return self.system().get(); }, py::return_internal_reference<>()))
        ;

}

}
