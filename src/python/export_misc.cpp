#include <memory>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/tuple.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/exception_translator.hpp>

#include "util.h"
#include "errors.h"
#include <common/notifysubscription.h>
#include <robotdebug.h>

namespace py = boost::python;

namespace Robot::Python {

static void translate(const attribute_error &e) 
{
    PyErr_SetString(PyExc_AttributeError, e.what());
}


void export_misc() 
{
    py::register_exception_translator<attribute_error>(&translate);

    py::class_<WithNotifyDefault, boost::noncopyable>("Subscribable", py::no_init)
        .add_static_property("NOTIFY_DEFAULT", py::make_getter(WithNotifyDefault::NOTIFY_DEFAULT))
        .def("subscribe", +[](WithNotifyDefault &self) { return notify_subscribe(self); })
        .def("subscribe", +[](WithNotifyDefault &self, const py::tuple &events) { return notify_subscribe(self, tuple_to_container<NotifySubscription<WithNotifyDefault::notify_type>::result_type>(events)); })
        .def("subscribe", +[](WithNotifyDefault &self, std::shared_ptr<NotifySubscription<WithNotifyDefault::notify_type>> sub, int offset) { notify_attach(sub, self, offset); return sub; })
        .def("subscribe", +[](WithNotifyDefault &self, std::shared_ptr<NotifySubscription<WithNotifyDefault::notify_type>> sub, int offset, const py::tuple &events) { notify_attach(sub, self, offset, tuple_to_container<NotifySubscription<WithNotifyDefault::notify_type>::result_type>(events)); return sub; })
        ;

    py::class_<NotifySubscriptionDefault, std::shared_ptr<NotifySubscriptionDefault>, boost::noncopyable>("Subscription", py::no_init)
        .add_property("fd", &NotifySubscriptionDefault::get_fd)
        .def("unsubscribe", &NotifySubscriptionDefault::unsubscribe)
        .def("clear", &NotifySubscriptionDefault::clear)
        .def("read", +[](NotifySubscriptionDefault &sub) { return container_to_tuple(sub.read()); })
        .def("read", +[](NotifySubscriptionDefault &sub, float timeout) { return container_to_tuple(sub.read(std::chrono::milliseconds((uint)(1000.0f*timeout)))); })
        ;

    #ifdef ROBOT_DEBUG
    using Robot::Debug::TestComponent;
    py::class_<TestComponent, std::shared_ptr<TestComponent>, py::bases<WithNotifyDefault>, boost::noncopyable>("TestComponent")
        .def("ping", &TestComponent::ping)
        .def("__str__", +[](const NotifySubscriptionDefault &sub) { return "<TestComponent>"; })
        .def("__enter__", +[](TestComponent &self) {
            self.mutex_lock();
            return self.shared_from_this();
        })
        .def("__exit__", +[](TestComponent &self, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            self.mutex_unlock();
        })
        ;
    #endif


}

}