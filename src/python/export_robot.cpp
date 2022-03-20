#include <memory>
#include <variant>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/tuple.hpp>
#include <boost/python/dict.hpp>


#include <common/notifysubscription.h>
#include <robot.h>
#include <robotcontext.h>
#include <rc/receiver.h>
#include <telemetry/telemetry.h>
#include <motor/control.h>
#include <led/control.h>
#include <kinematic/kinematic.h>
#include <input/control.h>
#include <system/power.h>
#include <system/network.h>

#include "util.h"

namespace py = boost::python;


namespace Robot::Python {


class PropertiesWrapper {
    public:
        PropertiesWrapper(const std::shared_ptr<::Robot::Context> &context) : m_context { context } {}
        PropertiesWrapper(const PropertiesWrapper &other) : m_context { other.m_context } {}
        virtual ~PropertiesWrapper() {}

        py::list groups() const
        {
            using ::Robot::Context;
            const Context::guard _lock(m_context->mutex());
            const auto &properties = m_context->properties();
            py::list res;
            for (const auto &prop : properties) {
                res.append(prop.first);
            }
            return res;
        }

        py::dict get(const std::string &key) const
        {
            using ::Robot::Context;
            const Context::guard _lock(m_context->mutex());
            const auto &properties = m_context->properties(key);
            py::dict res;
            for (const auto &v : properties) {
                if (std::holds_alternative<int>(v.second)) {
                    res[v.first] = std::get<int>(v.second);
                }
                else if (std::holds_alternative<std::string>(v.second)) {
                    res[v.first] = std::get<std::string>(v.second);
                }
                else if (std::holds_alternative<float>(v.second)) {
                    res[v.first] = std::get<float>(v.second);
                }
                else if (std::holds_alternative<bool>(v.second)) {
                    res[v.first] = std::get<bool>(v.second);
                }
                else {
                    BOOST_THROW_EXCEPTION(std::domain_error("Unknown property value type"));
                }
            }
            return res;
        }

        void update(const std::string &key, const py::dict &values)
        {
            using ::Robot::Context;
            const Context::guard _lock(m_context->mutex());
            auto &properties = m_context->properties(key);

            const auto items = values.items();
            BOOST_LOG_TRIVIAL(info) << "Update " << py::len(items);
            for(ssize_t i = 0; i < py::len(items); i++) {
                py::extract<std::string> key(items[i][0]);
                py::object value = items[i][1];
                if (key.check()) {
                    std::string name = key;
                    auto &target = properties.at(name);
                    if (std::holds_alternative<int>(target)) {
                        py::extract<int> _v(value);
                        if (_v.check()) {
                            target = static_cast<int>(_v);
                        }
                    }
                    else if (std::holds_alternative<float>(target)) {
                        py::extract<float> _v(value);
                        if (_v.check()) {
                            target = static_cast<float>(_v);
                        }
                    }
                    else if (std::holds_alternative<std::string>(target)) {
                        py::extract<std::string> _v(value);
                        if (_v.check()) {
                            target = static_cast<std::string>(_v);
                        }
                    }
                    else if (std::holds_alternative<bool>(target)) {
                        py::extract<bool> _v(value);
                        if (_v.check()) {
                            target = static_cast<bool>(_v);
                        }
                    }
                }
            }
        }

    private:
        std::shared_ptr<::Robot::Context> m_context;
};

void export_robot() 
{
    using namespace ::Robot;

    py::class_<PropertiesWrapper>("RobotProperties", py::no_init)
        .def("__getitem__", &PropertiesWrapper::get)
        .def("__setitem__", &PropertiesWrapper::update)
        .def("groups", &PropertiesWrapper::groups)
        .def("update", &PropertiesWrapper::update)
        .def("get", &PropertiesWrapper::get)
        ;

    py::class_<Robot, std::shared_ptr<Robot>, py::bases<WithNotifyInt>, boost::noncopyable>("Robot")
        .add_property("heartbeat", +[](const Robot &r) { return r.context()->heartbeat(); })
        .add_property("rc_receiver", py::make_function(+[](const Robot &r){ return r.rcReceiver().get(); }, py::return_internal_reference<>()))
        .add_property("motor_control", py::make_function(+[](const Robot &r){ return r.motorControl().get(); }, py::return_internal_reference<>()))
        .add_property("led_control", py::make_function(+[](const Robot &r){ return r.ledControl().get(); }, py::return_internal_reference<>()))
        .add_property("telemetry", py::make_function(+[](const Robot &r){ return r.telemetry().get(); }, py::return_internal_reference<>()))
        .add_property("kinematic", py::make_function(+[](const Robot &r){ return r.kinematic().get(); }, py::return_internal_reference<>()))
        .add_property("input", py::make_function(+[](const Robot &r){ return r.input().get(); }, py::return_internal_reference<>()))
        .add_property("power", py::make_function(+[](const Robot &r){ return r.power().get(); }, py::return_internal_reference<>()))
        .add_property("network", py::make_function(+[](const Robot &r){ return r.network().get(); }, py::return_internal_reference<>()))
        .add_property("properties", +[](const Robot &r) { return PropertiesWrapper(r.context()); })
        .def("init", &Robot::init)
        .def("cleanup", &Robot::cleanup)
        ;

}

}