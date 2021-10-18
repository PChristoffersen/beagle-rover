#include <boost/log/trivial.hpp>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "util.h"
#include "../motor/motor.h"
#include "../motor/motorcontrol.h"

using namespace boost;
using namespace boost::python;


void python_export_motor() {
    register_ptr_to_python<shared_ptr<Motor> >();
    register_ptr_to_python<shared_ptr<MotorControl> >();

    class_<Motor, noncopyable>("Motor", no_init)
        .add_property("index", &Motor::getIndex)
        ;
    class_<MotorControl::MotorList>("MotorList")
        .def(vector_indexing_suite<MotorControl::MotorList, true>() )
        ;
    class_<MotorControl, noncopyable>("MotorControl", no_init)
        .add_property("motors", &MotorControl::getMotors)
        ;


}