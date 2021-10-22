#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "util.h"
#include "../kinematic/kinematic.h"

using namespace boost;
using namespace boost::python;


void python_export_kinematic() {
    register_ptr_to_python<shared_ptr<Kinematic> >();

    enum_<Kinematic::SteeringMode>("SteeringMode")
        .export_values()
        ;
    enum_<Kinematic::DriveMode>("DriveMode")
        .value("NONE", Kinematic::DRIVE_NONE)
        .value("NORMAL", Kinematic::DRIVE_NORMAL)
        .value("SPINNING", Kinematic::DRIVE_SPINNING)
        .value("BALANCING", Kinematic::DRIVE_BALANCING)
        ;
    enum_<Kinematic::Orientation>("Orientation")
        .value("NORTH", Kinematic::ORIENTATION_NORTH)
        .value("SOUTH", Kinematic::ORIENTATION_SOUTH)
        .value("EAST", Kinematic::ORIENTATION_EAST)
        .value("WEST", Kinematic::ORIENTATION_WEST)
        ;

    class_<Kinematic, noncopyable>("Kinematic", no_init)
        .add_property("steering_mode", &Kinematic::getSteeringMode, &Kinematic::setSteeringMode)
        .add_property("drive_mode", &Kinematic::getDriveMode, &Kinematic::setDriveMode)
        ;
}