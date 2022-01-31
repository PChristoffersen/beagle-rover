#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include "util.h"



namespace Robot::Python {
    void export_util();
    void export_motor();
    void export_telemetry();
    void export_rcreceiver();
    void export_led();
    void export_kinematic();
    void export_robot();
    void export_input();
};


BOOST_PYTHON_MODULE(robotsystem) 
{
    using namespace Robot::Python;

    export_util();
    export_motor();
    export_telemetry();
    export_rcreceiver();
    export_led();
    export_kinematic();
    export_input();
    export_robot();
}