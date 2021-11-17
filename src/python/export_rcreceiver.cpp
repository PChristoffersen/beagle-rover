#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "util.h"
#include "../rcreceiver/rcreceiver.h"

using namespace std;
namespace py = boost::python;


void python_export_rcreceiver() 
{

    py::register_ptr_to_python<shared_ptr<RCReceiver> >();

    py::class_<RCReceiver, boost::noncopyable>("RCReceiver", py::no_init)
        .add_property("connected", &RCReceiver::isConnected)
        .add_property("flags", &RCReceiver::getFlags)
        .add_property("rssi", &RCReceiver::getRSSI)
        .def("__enter__", +[](RCReceiver &rc) {
            rc.lock();
            return rc.shared_from_this();
        })
        .def("__exit__", +[](RCReceiver &rc, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            rc.unlock();
        })
        ;
}