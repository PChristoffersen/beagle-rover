#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include <robotconfig.h>
#include <common/notifysubscription.h>
#include <rc/receiver.h>
#include "util.h"

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

    py::class_<Receiver, std::shared_ptr<Receiver>, py::bases<WithNotifyDefault>, boost::noncopyable>("RCReceiver", py::no_init)
        .add_property("enabled", &Receiver::getEnabled, &Receiver::setEnabled)
        .add_property("connected", &Receiver::isConnected)
        .add_property("flags", &Receiver::getFlags)
        .add_property("rssi", &Receiver::getRSSI)
        .def("__enter__", +[](Receiver &self) {
            self.mutex_lock();
            return self.shared_from_this();
        })
        .def("__exit__", +[](Receiver &self, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            self.mutex_unlock();
        })
        ;
    
    #else

    py::class_<DummyComponent, boost::noncopyable>("RCReceiver", py::no_init)
        ;

    #endif
}

}
