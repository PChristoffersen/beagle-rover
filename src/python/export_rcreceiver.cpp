#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/numpy.hpp>
#include <boost/python/tuple.hpp>

#include <robotconfig.h>
#include <common/notifysubscription.h>
#include <rc/receiver.h>
#include "util.h"

namespace py = boost::python;
namespace np = boost::python::numpy;

namespace Robot::Python {

static np::ndarray channelsToNumpy(const Robot::RC::ChannelList &channels, const py::object &obj) {
    using Robot::RC::ChannelList;
    using value_type = std::uint32_t;

    // Being a bit naughty using Robot::Value class as an integer, so assert that they are the same size
    assert(sizeof(ChannelList::value_type)==sizeof(value_type));
    auto shape = py::make_tuple(channels.count());
    auto strides = py::make_tuple(sizeof(value_type));
    auto dtype = np::dtype::get_builtin<value_type>();

    return np::from_data(channels.data(), dtype, shape, strides, obj);
}


void export_rcreceiver() 
{
    #if ROBOT_HAVE_RC
    
    using Robot::RC::Receiver, Robot::RC::Flags, Robot::RC::ChannelList;

    py::class_<Flags>("RCFlags", py::no_init)
        //.add_property("ch17", &Flags::ch17)
        //.add_property("ch18", &Flags::ch18)
        .add_property("frame_lost", &Flags::frameLost)
        .add_property("failsafe", &Flags::failsafe)
        ;

    py::class_<Receiver, std::shared_ptr<Receiver>, py::bases<WithNotifyInt, WithMutexStd>, boost::noncopyable>("RCReceiver", py::no_init)
        .add_static_property("NOTIFY_CHANNELS", py::make_getter(Receiver::NOTIFY_CHANNELS))
        .add_property("enabled", &Receiver::getEnabled, &Receiver::setEnabled)
        .add_property("connected", &Receiver::isConnected)
        .add_property("flags", &Receiver::getFlags)
        .add_property("rssi", &Receiver::getRSSI)
        .add_property("channels", +[](const py::object &obj) {
            const std::shared_ptr<Receiver> self = py::extract<const std::shared_ptr<Receiver>>(obj);
            const Receiver::guard _lock(self->mutex());
            return channelsToNumpy(self->channels(), obj);
        })
        ;
    
    #else

    py::class_<DummyComponent, boost::noncopyable>("RCReceiver", py::no_init)
        ;

    #endif
}

}
