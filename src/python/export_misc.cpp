#include <memory>
#include <boost/format.hpp>
#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/tuple.hpp>
#include <boost/python/implicit.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "util.h"
#include "errors.h"
#include <common/notifysubscription.h>
#include <common/withmutex.h>
#include <robotdebug.h>
#include <config.h>

namespace py = boost::python;

namespace Robot::Python {

static void translate(const attribute_error &e) 
{
    PyErr_SetString(PyExc_AttributeError, e.what());
}


void export_misc() 
{
    py::register_exception_translator<attribute_error>(&translate);

    py::def("robot_version", +[]() { return Robot::Config::VERSION; });
    py::def("robot_version_full", +[]() { return Robot::Config::VERSION_FULL; });

    py::class_<WithNotifyInt, boost::noncopyable>("Subscribable", py::no_init)
        .add_static_property("NOTIFY_DEFAULT", py::make_getter(WithNotifyInt::NOTIFY_DEFAULT))
        .def("subscribe", +[](WithNotifyInt &self) { return notify_subscribe(self); })
        .def("subscribe", +[](WithNotifyInt &self, const py::tuple &events) { return notify_subscribe(self, tuple_to_container<NotifySubscription<WithNotifyInt::notify_type>::result_type>(events)); })
        .def("subscribe", +[](WithNotifyInt &self, std::shared_ptr<NotifySubscription<WithNotifyInt::notify_type>> sub, int offset) { notify_attach(sub, self, offset); return sub; })
        .def("subscribe", +[](WithNotifyInt &self, std::shared_ptr<NotifySubscription<WithNotifyInt::notify_type>> sub, int offset, const py::tuple &events) { notify_attach(sub, self, offset, tuple_to_container<NotifySubscription<WithNotifyInt::notify_type>::result_type>(events)); return sub; })
        ;

    py::class_<NotifySubscriptionInt, std::shared_ptr<NotifySubscriptionInt>, boost::noncopyable>("Subscription", py::no_init)
        .add_property("fd", &NotifySubscriptionInt::get_fd)
        .def("unsubscribe", &NotifySubscriptionInt::unsubscribe)
        .def("clear", &NotifySubscriptionInt::clear)
        .def("read", +[](NotifySubscriptionInt &sub) { return container_to_tuple(sub.read()); })
        .def("read", +[](NotifySubscriptionInt &sub, float timeout) { return container_to_tuple(sub.read(std::chrono::milliseconds((uint)(1000.0f*timeout)))); })
        ;


    py::class_<WithMutexStd, boost::noncopyable>("WithMutexStd", py::no_init) 
        .def("__enter__", +[](WithMutexStd &self) {
            self.mutex_lock();
        })
        .def("__exit__", +[](WithMutexStd &self, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            self.mutex_unlock();
        })
        ;

    #ifdef ROBOT_DEBUG
    using Robot::Debug::TestComponent;
    py::class_<TestComponent, std::shared_ptr<TestComponent>, py::bases<WithNotifyInt, WithMutexStd>, boost::noncopyable>("TestComponent")
        .def("ping", &TestComponent::ping)
        .def("__str__", +[](const NotifySubscriptionInt &sub) { return "<TestComponent>"; })
        ;
    #endif


}

}