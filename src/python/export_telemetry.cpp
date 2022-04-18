#include <typeinfo>
#include <vector>
#include <sstream>
#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/numpy.hpp>
#include <boost/python/dict.hpp>

#include "util.h"
#include <common/notifysubscription.h>
#include <telemetry/telemetry.h>

namespace py = boost::python;
namespace np = boost::python::numpy;

namespace Robot::Python {



static void set_dict_value(py::dict &out, const std::string key, const Robot::Telemetry::Value &value) 
{
    if (std::holds_alternative<std::string>(value)) {
        out[key] = std::get<std::string>(value);
        return;
    }
    if (std::holds_alternative<bool>(value)) {
        out[key] = std::get<bool>(value);
        return;
    }
    if (std::holds_alternative<double>(value)) {
        out[key] = std::get<double>(value);
        return;
    }
    if (std::holds_alternative<float>(value)) {
        out[key] = std::get<float>(value);
        return;
    }
    if (std::holds_alternative<std::uint32_t>(value)) {
        out[key] = std::get<std::uint32_t>(value);
        return;
    }
    if (std::holds_alternative<std::int32_t>(value)) {
        out[key] = std::get<std::int32_t>(value);
        return;
    }
    BOOST_THROW_EXCEPTION(std::domain_error("Unknown property value type for property: "+key));
}

static void map2dict(py::dict &out, const Robot::Telemetry::ValueMap &in) 
{
    for (const auto &e : in) {
        auto &key = e.first;
        set_dict_value(out, key, e.second);
    }
}


template<typename T>
static py::tuple to_2d_array(const T &history, const py::object &owner, np::dtype dtype) {
    auto shape = py::make_tuple(history.values.size(), history.values[0].size());
    auto strides = py::make_tuple(sizeof(history.values[0]), sizeof(history.values[0][0]));
    return py::make_tuple(history.head, np::from_data(history.values.data(), dtype, shape, strides, owner));
}


void export_telemetry() 
{
    using Robot::Telemetry::Telemetry, Robot::Telemetry::HistoryMotorDuty;

  
    py::class_<Telemetry, std::shared_ptr<Telemetry>, py::bases<WithNotifyInt, WithMutexStd>, boost::noncopyable>("Telemetry", py::no_init)
        .add_static_property("NOTIFY_IMU", py::make_getter(Telemetry::NOTIFY_IMU))
        .add_static_property("NOTIFY_ODOMETER", py::make_getter(Telemetry::NOTIFY_ODOMETER))
        .add_property("imu", +[](Telemetry &self){ 
            Telemetry::guard(self.mutex());
            py::dict vals;
            map2dict(vals, self.imuValues());
            return vals; 
        })
        .add_property("odometer", +[](Telemetry &self){ 
            Telemetry::guard(self.mutex());
            py::dict vals;
            map2dict(vals, self.odometerValues());
            return vals; 
        })
        .add_property("history_time_ms", &Telemetry::historyLastMS)
        .add_property("history_interval_ms", &Telemetry::historyIntervalMS)
        .add_property("history_imu", +[](const py::object &obj){ 
            const std::shared_ptr<Telemetry> self = py::extract<const std::shared_ptr<Telemetry>>(obj);
            return to_2d_array(self->historyIMU(), obj, np::dtype::get_builtin<float>());
        })
        .add_property("history_motor_duty", +[](const py::object &obj){ 
            const std::shared_ptr<Telemetry> self = py::extract<const std::shared_ptr<Telemetry>>(obj);
            return to_2d_array(self->historyMotorDuty(), obj, np::dtype::get_builtin<float>());
        })
        .add_property("history_motor_rpm", +[](const py::object &obj){ 
            const std::shared_ptr<Telemetry> self = py::extract<const std::shared_ptr<Telemetry>>(obj);
            return to_2d_array(self->historyMotorRPM(), obj, np::dtype::get_builtin<float>());
        })
        ;

}

}