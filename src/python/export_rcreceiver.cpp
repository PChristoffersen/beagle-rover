#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include <robotconfig.h>
#include <rc/receiver.h>
#include "util.h"
#include "subscription.h"

namespace py = boost::python;

namespace Robot::Python {

void export_rcreceiver() 
{
    #if ROBOT_HAVE_RC
    
    using Robot::RC::Receiver, Robot::RC::Flags;

    py::class_<Flags>("RCFlags", py::no_init)
        //.add_property("ch17", &Flags::ch17)
        //.add_property("ch18", &Flags::ch18)
        .add_property("frame_lost", &Flags::frameLost)
        .add_property("failsafe", &Flags::failsafe)
        ;

    py::class_<Receiver, std::shared_ptr<Receiver>, boost::noncopyable>("RCReceiver", py::no_init)
        .add_static_property("NOTIFY_DEFAULT", py::make_getter(Receiver::NOTIFY_DEFAULT))
        .add_property("enabled", &Receiver::getEnabled, &Receiver::setEnabled)
        .add_property("connected", &Receiver::isConnected)
        .add_property("flags", &Receiver::getFlags)
        .add_property("rssi", &Receiver::getRSSI)
        .def("subscribe", +[](Receiver &receiver) { return notify_subscribe<Receiver>(receiver); })
        .def("subscribe_attach", +[](Receiver &receiver, NotifySubscription<Receiver::NotifyType> &sub, int offset) { return notify_attach<Receiver>(sub, receiver, offset); })
        .def("__enter__", +[](Receiver &rc) {
            rc.mutex_lock();
            return rc.shared_from_this();
        })
        .def("__exit__", +[](Receiver &rc, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            rc.mutex_unlock();
        })
        ;
    
    #else

    py::class_<DummyComponent, boost::noncopyable>("RCReceiver", py::no_init)
        ;

    #endif
}

}
