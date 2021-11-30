#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "util.h"
#include "../kinematic/kinematic.h"

using namespace std;
using namespace Robot::Kinematic;
namespace py = boost::python;


void python_export_kinematic() 
{
    py::enum_<SteeringMode>("SteeringMode")
        .value("NONE", SteeringMode::NONE)
        .value("FRONT", SteeringMode::FRONT)
        .value("REAR", SteeringMode::REAR)
        .value("ALL", SteeringMode::ALL)
        .value("SKID", SteeringMode::SKID)
        ;
    py::enum_<DriveMode>("DriveMode")
        .value("NONE", DriveMode::NONE)
        .value("STANDARD", DriveMode::STANDARD)
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

    py::class_<Kinematic, shared_ptr<Kinematic>, boost::noncopyable>("Kinematic", py::no_init)
        .add_property("steering_mode", &Kinematic::getSteeringMode, &Kinematic::setSteeringMode)
        .add_property("drive_mode", &Kinematic::getDriveMode, &Kinematic::setDriveMode)
        .add_property("orientation", &Kinematic::getOrientation, &Kinematic::setOrientation)
        .def("__enter__", +[](Kinematic &kinematic) {
            kinematic.lock();
            return kinematic.shared_from_this();
        })
        .def("__exit__", +[](Kinematic &kinematic, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            kinematic.unlock();
        })
        ;
}