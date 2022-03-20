#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

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


    py::enum_<Network::Interface::Type>("NetworkInterfaceType")
        .value("ETHERNET", Network::Interface::Type::ETHERNET)
        .value("WIRELESS", Network::Interface::Type::WIRELESS)
        .value("WIRELESS_AP", Network::Interface::Type::WIRELESS_AP)
        .value("USB_GADGET", Network::Interface::Type::USB_GADGET)
        ;

    py::class_<Network::Interface::WifiInfo, boost::noncopyable>("NetworkWifiInfo", py::no_init)
        .add_property("ssid", +[](const Network::Interface::WifiInfo &self) { return self.ssid; })
        .add_property("frequency", +[](const Network::Interface::WifiInfo &self) { return self.frequency; })
        .add_property("channel", +[](const Network::Interface::WifiInfo &self) { return self.channel; })
        .add_property("channel_width", +[](const Network::Interface::WifiInfo &self) { return self.channel_width; })
        .add_property("signal_dbm", +[](const Network::Interface::WifiInfo &self) { return self.signal_dbm; })
        .add_property("signal_rssi", +[](const Network::Interface::WifiInfo &self) { return self.signal_rssi; })
        ;

    py::class_<Network::Interface, std::shared_ptr<Network::Interface>, py::bases<WithMutexStd>, boost::noncopyable>("NetworkInterface", py::no_init)
        .add_property("name", +[](const Network::Interface &self) { return self.name; })
        .add_property("type", +[](const Network::Interface &self) { return self.type; })
        .add_property("mac", +[](const Network::Interface &self) { return self.mac; })
        .add_property("active", +[](const Network::Interface &self) { return self.active; })
        .add_property("addresses", +[](const Network::Interface &self) { return container_to_tuple(self.addresses); })
        .add_property("wifi", py::make_function(+[](const Network::Interface &self){ return self.wifi.get(); }, py::return_internal_reference<>()))
        ;

    py::class_<Network::interface_list_type>("InterfaceList")
        .def(py::vector_indexing_suite<Network::interface_list_type, true>())
        ;

    py::class_<Network, std::shared_ptr<Network>, py::bases<WithNotifyInt, WithMutexStd>, boost::noncopyable>("Network", py::no_init)
        .add_static_property("NOTIFY_SIGNAL", py::make_getter(Network::NOTIFY_SIGNAL))
        .def("interfaces", +[](const Network &self) { return self.interfaces(); })
        .def("interfaces", +[](const Network &self, Network::Interface::Type type) { return self.interfaces(type); })
        ;



    py::enum_<PowerSource::Type>("PowerSourceType")
        .value("UNKNOWN", PowerSource::Type::UNKNOWN)
        .value("BATTERY", PowerSource::Type::BATTERY)
        ;

    py::class_<PowerSource, py::bases<WithMutexStd>, boost::noncopyable>("PowerSource", py::no_init) 
        .add_property("name", &PowerSource::name)
        .add_property("type", &PowerSource::type)
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

    py::class_<Power, std::shared_ptr<Power>, py::bases<WithNotifyInt>, boost::noncopyable>("Power", py::no_init)
        .add_property("system", py::make_function(+[](const Power &self){ return self.system().get(); }, py::return_internal_reference<>()))
        ;

}

}
