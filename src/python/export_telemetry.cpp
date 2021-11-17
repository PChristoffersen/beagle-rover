#include <typeinfo>
#include <vector>
#include <boost/python.hpp>
#include <boost/log/trivial.hpp>

#include "../telemetry/telemetry.h"
#include "util.h"

using namespace std;
namespace py = boost::python;


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

        virtual void on_event(const TelemetryEvent &event) = 0;

        void event(const TelemetryEvent &event) {
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
        virtual void on_event(const TelemetryEvent &event) override {
            if (auto f = this->get_override("on_event")) {
                if (const auto ev = dynamic_cast<const TelemetryEventBattery*>(&event)) {
                    f(*ev);
                }
                else {
                    BOOST_LOG_TRIVIAL(warning) << "Unknown telemetry event type " << typeid(event).name();
                }
            }
        }
};


void python_export_telemetry() 
{
    py::register_ptr_to_python<shared_ptr<Telemetry> >();

    py::class_<Telemetry, boost::noncopyable>("Telemetry", py::no_init)
        ;
        
    py::class_<TelemetryListenerWrap, boost::noncopyable>("TelemetryListener")
        .def("connect", &TelemetryListenerWrap::connect)
        .def("disconnect", &TelemetryListenerWrap::disconnect)
        .def("on_event", pure_virtual(&TelemetryListenerWrap::on_event))
        ;

    py::class_<TelemetryEvent>("TelemetryEvent", py::no_init)
        ;

    py::class_<TelemetryEventBattery, py::bases<TelemetryEvent>>("TelemetryEventBattery")
        .def_readonly("battery_id", &TelemetryEventBattery::battery_id)
        .def_readonly("cell_voltages", &TelemetryEventBattery::cell_voltage)
        ;

}