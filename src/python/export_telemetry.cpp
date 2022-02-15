#include <typeinfo>
#include <vector>
#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/dict.hpp>

#include "util.h"
#include "subscription.h"
#include <telemetry/telemetry.h>

namespace py = boost::python;

namespace Robot::Python {

#if 0
class TelemetryListener {
    public:
        virtual ~TelemetryListener() {
            disconnect();
        }

        void connect(std::shared_ptr<Telemetry> telemetry) {
            BOOST_LOG_TRIVIAL(trace) << "Connect";
            m_connection.disconnect();
            m_connection = telemetry->sig_event.connect([&](const auto &e){ event(e); });
        }

        void disconnect() {
            BOOST_LOG_TRIVIAL(trace) << "Disconnect";
            m_connection.disconnect();
        }

        virtual void on_event(const Event &event) = 0;

        void event(const Event &event) {
            PyGILState_STATE gstate = PyGILState_Ensure();
            try {
                on_event(event);
            }
            catch (const py::error_already_set &err) {
                BOOST_LOG_TRIVIAL(warning) << "Event error: " << parse_python_exception();
            }
            PyGILState_Release(gstate);
        }

    protected:
        boost::signals2::connection m_connection;
};

class TelemetryListenerWrap : public TelemetryListener, public boost::python::wrapper<TelemetryListener> {
    public:
        virtual void on_event(const Event &event) override {
            if (auto f = this->get_override("on_event")) {
                if (const auto ev = dynamic_cast<const EventBattery*>(&event)) {
                    f(*ev);
                }
                else {
                    BOOST_LOG_TRIVIAL(warning) << "Unknown telemetry event type " << typeid(event).name();
                }
            }
        }
};
#endif


static void set_dict_value(py::dict &out, const std::string &key, const Robot::Telemetry::Value &value) 
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

  
    py::class_<Telemetry, std::shared_ptr<Telemetry>, boost::noncopyable>("Telemetry", py::no_init)
        .add_property("values", +[](Telemetry &telemetry){ 
            Telemetry::guard(telemetry.mutex());
            py::dict vals;
            map2dict(vals, telemetry.valuesUnlocked());
            vals["version"] = telemetry.valuesVersion();
            return vals; 
        })
        .add_property("values_version", &Telemetry::valuesVersion)
        //.def("subscribe", +[](Telemetry &telemetry) { return notify_subscribe(telemetry); })
        ;

}

}