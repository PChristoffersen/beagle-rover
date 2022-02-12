#include <boost/log/trivial.hpp>

#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/python.hpp>
#undef BOOST_ALLOW_DEPRECATED_HEADERS

#include <robotconfig.h>
#include <system/network.h>
#include <system/power.h>
#include "util.h"
#include "subscription.h"

namespace py = boost::python;

namespace Robot::Python {

void export_system() 
{
    using Robot::System::Network, Robot::System::Power;

    py::class_<Network, std::shared_ptr<Network>, boost::noncopyable>("Network", py::no_init)
        ;

    py::class_<Power, std::shared_ptr<Power>, boost::noncopyable>("Power", py::no_init)
        ;

}

}
