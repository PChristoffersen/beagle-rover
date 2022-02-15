#define BOOST_TEST_MODULE Robot
#include <boost/test/included/unit_test.hpp>

#include <thread>
#include <boost/asio.hpp>

#include <robot.h>

using namespace std::literals;

static Robot::Logging::LogInit __log_init;

BOOST_AUTO_TEST_SUITE(robot_suite)

BOOST_AUTO_TEST_CASE(TestCreate)
{
    auto robot { std::make_unique<Robot::Robot>() };
}

BOOST_AUTO_TEST_CASE(TestCreateInit)
{
    auto robot { std::make_unique<Robot::Robot>() };
    robot->init();
}

BOOST_AUTO_TEST_CASE(TestCreateInitCleanup)
{
    auto robot { std::make_unique<Robot::Robot>() };
    robot->init();
    std::this_thread::sleep_for(1s);
    robot->cleanup();
}

BOOST_AUTO_TEST_SUITE_END()

