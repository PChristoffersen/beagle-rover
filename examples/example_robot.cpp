#include <memory>
#include <iostream>
#include <exception>
#include <chrono>
#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/exception/all.hpp>
#include <boost/log/trivial.hpp>
#include <boost/bind.hpp>

#include <robot.h>

using namespace std::literals;

int main() {
    using boost::asio::io_context, boost::asio::signal_set;

    io_context io;

    signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(boost::bind(&io_context::stop, &io));

    Robot::Logging::initLogging();

    BOOST_LOG_TRIVIAL(info) << "Creating robot";

    auto robot { std::make_unique<Robot::Robot>() };

    try {
        BOOST_LOG_TRIVIAL(info) << "Initializing";
        robot->init();

        BOOST_LOG_TRIVIAL(info) << "Running";

        io.run_for(10s);

        BOOST_LOG_TRIVIAL(info) << "Stopping...";

    }
    catch (std::exception const &e) {
        BOOST_LOG_TRIVIAL(error) << boost::diagnostic_information(e);
    }

    BOOST_LOG_TRIVIAL(info) << "Cleanup";
    robot->cleanup();
    BOOST_LOG_TRIVIAL(info) << "Done";

    return 0;
}