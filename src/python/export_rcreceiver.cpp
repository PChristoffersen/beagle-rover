#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include <robotconfig.h>
#include <rc/receiver.h>
#include "util.h"

namespace py = boost::python;

namespace Robot::Python {

void export_rcreceiver() 
{
    #if ROBOT_HAVE_RC
    
    using Robot::RC::Receiver;

    py::class_<Receiver, std::shared_ptr<Receiver>, boost::noncopyable>("RCReceiver", py::no_init)
        .add_property("enabled", &Receiver::getEnabled, &Receiver::setEnabled)
        .add_property("connected", &Receiver::isConnected)
        .add_property("flags", &Receiver::getFlags)
        .add_property("rssi", &Receiver::getRSSI)
        //.def("subscribe", +[](Receiver &receiver, py::object &func) { return notify_subscribe<Receiver>(receiver, func); })
        .def("__enter__", +[](Receiver &rc) {
            rc.lock();
            return rc.shared_from_this();
        })
        .def("__exit__", +[](Receiver &rc, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            rc.unlock();
        })
        ;
    
    #endif
}

}
