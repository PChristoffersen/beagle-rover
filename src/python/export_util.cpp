#include <typeinfo>
#include <vector>
#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/dict.hpp>

#include "util.h"

namespace py = boost::python;

namespace Robot::Python {

void export_util() 
{
    py::class_<NotifySubscription, std::shared_ptr<NotifySubscription>, boost::noncopyable>("Subscription", py::no_init)
        .add_property("subscribed", &NotifySubscription::subscribed)
        .def("unsubscribe", &NotifySubscription::unsubscribe)
        ;
}

}