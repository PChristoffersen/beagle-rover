#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python/numpy.hpp>

#include "util.h"



namespace Robot::Python {
    void export_misc();
    void export_motor();
    void export_telemetry();
    void export_rcreceiver();
    void export_led();
    void export_kinematic();
    void export_input();
    void export_system();
    void export_robot();
}


BOOST_PYTHON_MODULE(robotsystem) 
{
    using namespace Robot::Python;

    boost::python::numpy::initialize();

    export_misc();
    export_motor();
    export_telemetry();
    export_rcreceiver();
    export_led();
    export_kinematic();
    export_input();
    export_system();
    export_robot();
}
