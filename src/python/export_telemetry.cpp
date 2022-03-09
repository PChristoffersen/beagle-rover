#include <typeinfo>
#include <vector>
#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/dict.hpp>

#include "util.h"
#include <common/notifysubscription.h>
#include <telemetry/telemetry.h>

namespace py = boost::python;

namespace Robot::Python {



static void set_dict_value(py::dict &out, const std::string key, const Robot::Telemetry::Value &value) 
{
    if (std::holds_alternative<std::string>(value)) {
        out[key] = std::get<std::string>(value);
    }
    if (std::holds_alternative<bool>(value)) {
        out[key] = std::get<bool>(value);
    }
    else if (std::holds_alternative<double>(value)) {
        out[key] = std::get<double>(value);
    }
    else if (std::holds_alternative<float>(value)) {
        out[key] = std::get<float>(value);
    }
    else if (std::holds_alternative<std::uint32_t>(value)) {
        out[key] = std::get<std::uint32_t>(value);
    }
}

static void map2dict(py::dict &out, const Robot::Telemetry::ValueMap &in) 
{
    for (const auto &e : in) {
        auto &key = e.first;
        set_dict_value(out, key, e.second);
    }
}


void export_telemetry() 
{
    using Robot::Telemetry::Telemetry;

  
    py::class_<Telemetry, std::shared_ptr<Telemetry>, py::bases<WithNotifyInt>, boost::noncopyable>("Telemetry", py::no_init)
        .add_static_property("NOTIFY_IMU", py::make_getter(Telemetry::NOTIFY_IMU))
        .add_property("imu", +[](Telemetry &self){ 
            Telemetry::guard(self.mutex());
            py::dict vals;
            map2dict(vals, self.imuValues());
            return vals; 
        })
        ;

}

}