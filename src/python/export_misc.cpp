#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/tuple.hpp>

#include <common/notifysubscription.h>
#include "util.h"

namespace py = boost::python;

namespace Robot::Python {


void export_misc() 
{
    py::class_<NotifySubscriptionDefault, std::shared_ptr<NotifySubscriptionDefault>, boost::noncopyable>("Subscription", py::no_init)
        .add_property("fd", &NotifySubscriptionDefault::get_fd)
        .def("unsubscribe", &NotifySubscriptionDefault::unsubscribe)
        .def("clear", &NotifySubscriptionDefault::clear)
        .def("read", +[](NotifySubscriptionDefault &sub) { return container_to_tuple(sub.read()); })
        .def("read", +[](NotifySubscriptionDefault &sub, float timeout) { return container_to_tuple(sub.read(std::chrono::milliseconds((uint)(1000.0f*timeout)))); })
        .def("__str__", +[](const NotifySubscriptionDefault &sub) { return (boost::format("<Subscription %s fd=%d>") % sub.get_name() % sub.get_fd()).str(); })
        ;
}

}