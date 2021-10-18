#include <typeinfo>
#include <vector>
#include <boost/python.hpp>
#include <boost/log/trivial.hpp>

#include "../telemetry/telemetry.h"
#include "util.h"

using namespace boost;
using namespace boost::python;


class TelemetryListener {
    public:
        virtual ~TelemetryListener() {
            m_connection.disconnect();
        }

        virtual void on_event(const TelemetryEvent &event) = 0;

        void event(const TelemetryEvent &event) {
            PyGILState_STATE gstate = PyGILState_Ensure();
            try {
                on_event(event);
            }
            catch (const error_already_set &err) {
                BOOST_LOG_TRIVIAL(warning) << "Event error: " << parse_python_exception();
            }
            PyGILState_Release(gstate);
        }

    protected:
        boost::signals2::connection m_connection;
};


class TelemetryListenerWrap : public TelemetryListener, public boost::python::wrapper<TelemetryListener> {
    public:
        TelemetryListenerWrap(boost::shared_ptr<Telemetry> telemetry) { 
            m_connection = telemetry->sig_event.connect(boost::bind(&TelemetryListener::event, this, _1));
        }

        virtual void on_event(const TelemetryEvent &event) override {
            if (auto f = this->get_override("on_event")) {
                if (typeid(event)==typeid(TelemetryEventBattery)) {
                    f((const TelemetryEventBattery&)event);
                }
                else {
                    BOOST_LOG_TRIVIAL(warning) << "Unknown telemetry event type " << typeid(event).name();
                }
            }
        }
};


void python_export_telemetry() {
    register_ptr_to_python<shared_ptr<Telemetry> >();

    class_<Telemetry, noncopyable>("Telemetry", no_init)
        ;
    class_<TelemetryListenerWrap, noncopyable>("TelemetryListener", init<shared_ptr<Telemetry> >())
        .def("on_event", pure_virtual(&TelemetryListenerWrap::on_event))
        ;
    class_<TelemetryEvent>("TelemetryEvent", no_init)
        ;
    class_<TelemetryEventBattery, bases<TelemetryEvent>>("TelemetryEventBattery")
        .def_readonly("battery_id", &TelemetryEventBattery::battery_id)
        .def_readonly("cell_voltages", &TelemetryEventBattery::cell_voltage)
        ;

}