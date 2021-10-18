#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "util.h"
#include "../rcreceiver.h"

using namespace boost;
using namespace boost::python;


void python_export_rcreceiver() {
    register_ptr_to_python<shared_ptr<RCReceiver> >();

    class_<RCReceiver, noncopyable>("RCReceiver", no_init)
        .add_property("connected", &RCReceiver::isConnected)
        .add_property("flags", &RCReceiver::getFlags)
        .add_property("rssi", &RCReceiver::getRSSI)
        ;
}