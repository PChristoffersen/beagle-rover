#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include <common/notifysubscription.h>
#include <kinematic/kinematic.h>
#include "util.h"

namespace py = boost::python;

namespace Robot::Python {

void export_kinematic() 
{
    using Robot::Kinematic::Kinematic;
    using Robot::Kinematic::DriveMode, Robot::Kinematic::Orientation;

    py::enum_<DriveMode>("DriveMode")
        .value("NONE", DriveMode::NONE)
        .value("ALL", DriveMode::ALL_WHEEL)
        .value("FRONT", DriveMode::FRONT_WHEEL)
        .value("REAR", DriveMode::REAR_WHEEL)
        .value("SKID", DriveMode::SKID)
        .value("SPINNING", DriveMode::SPINNING)
        .value("BALANCING", DriveMode::BALANCING)
        .value("PASSTHROUGH", DriveMode::PASSTHROUGH)
        ;
    py::enum_<Orientation>("Orientation")
        .value("NORTH", Orientation::NORTH)
        .value("SOUTH", Orientation::SOUTH)
        .value("EAST", Orientation::EAST)
        .value("WEST", Orientation::WEST)
        ;

    py::class_<Kinematic, std::shared_ptr<Kinematic>, boost::noncopyable>("Kinematic", py::no_init)
        .add_static_property("NOTIFY_DEFAULT", py::make_getter(Kinematic::NOTIFY_DEFAULT))
        .add_property("drive_mode", &Kinematic::getDriveMode, &Kinematic::setDriveMode)
        .add_property("orientation", &Kinematic::getOrientation, &Kinematic::setOrientation)
        .def("subscribe", +[](Kinematic &self) { return notify_subscribe(self); })
        .def("subscribe", +[](Kinematic &self, std::shared_ptr<NotifySubscription<Kinematic::NotifyType>> sub, int offset) { notify_attach(*sub, self, offset); return sub; })
        .def("__enter__", +[](Kinematic &self) {
            self.mutex_lock();
            return self.shared_from_this();
        })
        .def("__exit__", +[](Kinematic &self, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            self.mutex_unlock();
        })
        ;
}

}