#include <typeinfo>
#include <vector>
#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include "util.h"
#include <telemetry/telemetry.h>

namespace py = boost::python;

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



void python_export_telemetry() 
{
    using Robot::Telemetry::Telemetry;

  
    py::class_<Telemetry, std::shared_ptr<Telemetry>, boost::noncopyable>("Telemetry", py::no_init)
        ;

    #if 0        
    py::class_<TelemetryListenerWrap, boost::noncopyable>("TelemetryListener")
        .def("connect", &TelemetryListenerWrap::connect)
        .def("disconnect", &TelemetryListenerWrap::disconnect)
        .def("on_event", pure_virtual(&TelemetryListenerWrap::on_event))
        ;

    py::class_<Event>("TelemetryEvent", py::no_init)
        ;

    py::class_<EventBattery, py::bases<Event>>("TelemetryEventBattery")
        .def_readonly("battery_id", &EventBattery::battery_id)
        .def_readonly("cell_voltages", &EventBattery::cell_voltage)
        ;
    #endif

}