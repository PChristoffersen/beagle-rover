#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "util.h"
#include "../kinematic/kinematic.h"

using namespace std;
namespace py = boost::python;


void python_export_kinematic() 
{
    py::register_ptr_to_python<shared_ptr<Kinematic> >();

    py::enum_<Kinematic::SteeringMode>("SteeringMode")
        .value("NONE", Kinematic::SteeringMode::NONE)
        .value("FRONT", Kinematic::SteeringMode::FRONT)
        .value("REAR", Kinematic::SteeringMode::REAR)
        .value("ALL", Kinematic::SteeringMode::ALL)
        .value("SKID", Kinematic::SteeringMode::SKID)
        ;
    py::enum_<Kinematic::DriveMode>("DriveMode")
        .value("NONE", Kinematic::DriveMode::NONE)
        .value("NORMAL", Kinematic::DriveMode::NORMAL)
        .value("SPINNING", Kinematic::DriveMode::SPINNING)
        .value("BALANCING", Kinematic::DriveMode::BALANCING)
        .value("PASSTHROUGH", Kinematic::DriveMode::PASSTHROUGH)
        ;
    py::enum_<Kinematic::Orientation>("Orientation")
        .value("NORTH", Kinematic::Orientation::NORTH)
        .value("SOUTH", Kinematic::Orientation::SOUTH)
        .value("EAST", Kinematic::Orientation::EAST)
        .value("WEST", Kinematic::Orientation::WEST)
        ;

    py::class_<Kinematic, boost::noncopyable>("Kinematic", py::no_init)
        .add_property("steering_mode", &Kinematic::getSteeringMode, &Kinematic::setSteeringMode)
        .add_property("drive_mode", &Kinematic::getDriveMode, &Kinematic::setDriveMode)
        .def("__enter__", +[](Kinematic &kinematic) {
            kinematic.lock();
            return kinematic.shared_from_this();
        })
        .def("__exit__", +[](Kinematic &kinematic, const py::object &exc_type, const py::object &exc_val, const py::object &exc_tb) {
            kinematic.unlock();
        })
        ;
}