#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "util.h"
#include "../rc/rcreceiver.h"

using namespace std;
using namespace Robot::RC;
namespace py = boost::python;


void python_export_rcreceiver() 
{
    py::class_<Receiver, shared_ptr<Receiver>, boost::noncopyable>("RCReceiver", py::no_init)
        .add_property("enabled", &Receiver::getEnabled, &Receiver::setEnabled)
        .add_property("connected", &Receiver::isConnected)
        .add_property("flags", &Receiver::getFlags)
        .add_property("rssi", &Receiver::getRSSI)
        .def("__enter__", +[](Receiver &rc) {
            rc.lock();
            return rc.shared_from_this();
        })
        .def("__exit__", +[](Receiver &rc, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            rc.unlock();
        })
        ;
}