#define BOOST_TEST_MODULE Robot
#include <boost/test/included/unit_test.hpp>

#include <thread>
#include <boost/asio.hpp>

#include <robotcontext.h>
#include <robotlogging.h>
#include <system/network.h>

using namespace std::literals;

static Robot::Logging::LogInit __log_init;

BOOST_AUTO_TEST_SUITE(network_suite)

BOOST_AUTO_TEST_CASE(TestCreate)
{
    auto context { std::make_shared<Robot::Context>() };

    auto network { std::make_shared<Robot::System::Network>(context) };

    network->init();
    network->cleanup();
    
}


BOOST_AUTO_TEST_SUITE_END()

