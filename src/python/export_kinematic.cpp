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
    using namespace Kinematic;
    using Robot::Kinematic::Kinematic;

    py::enum_<DriveMode>("DriveMode")
        .value("NONE", DriveMode::NONE)
        .value("ALL", DriveMode::ALL_WHEEL)
        .value("FRONT", DriveMode::FRONT_WHEEL)
        .value("REAR", DriveMode::REAR_WHEEL)
        .value("SKID", DriveMode::SKID)
        .value("SPINNING", DriveMode::SPINNING)
        .value("BALANCING", DriveMode::BALANCING)
        ;
    py::enum_<Orientation>("Orientation")
        .value("NORTH", Orientation::NORTH)
        .value("SOUTH", Orientation::SOUTH)
        .value("EAST", Orientation::EAST)
        .value("WEST", Orientation::WEST)
        ;

    py::class_<Kinematic, std::shared_ptr<Kinematic>, py::bases<WithNotifyInt, WithMutexStd>, boost::noncopyable>("Kinematic", py::no_init)
        .add_static_property("NOTIFY_TELEMETRY", py::make_getter(Kinematic::NOTIFY_TELEMETRY))
        .add_property("drive_mode", &Kinematic::getDriveMode, &Kinematic::setDriveMode)
        .add_property("orientation", &Kinematic::getOrientation, &Kinematic::setOrientation)
        .add_property("odometer", &Kinematic::getOdometer)
        .def("reset_odometer", &Kinematic::resetOdometer)
        ;
}

}