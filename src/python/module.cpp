#include <boost/python.hpp>

#include "util.h"

using namespace boost;
using namespace boost::python;

void python_export_std();
void python_export_motor();
void python_export_telemetry();
void python_export_rcreceiver();
void python_export_led();
void python_export_kinematic();
void python_export_robot();


BOOST_PYTHON_MODULE(beaglerover) 
{
    PyEval_InitThreads();

    python_export_std();
    python_export_motor();
    python_export_telemetry();
    python_export_rcreceiver();
    python_export_led();
    python_export_kinematic();
    python_export_robot();

}