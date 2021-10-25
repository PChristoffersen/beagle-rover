#include <memory>
#include <iostream>
#include <exception>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/thread/thread.hpp>
#include <boost/exception/all.hpp>
#include <boost/bind.hpp>

#include "robot.h"
#include "robotcontext.h"
#include "kinematic/kinematic.h"




int main() {
    using namespace boost::asio;

    io_context io;


    signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(boost::bind(&io_context::stop, &io));

    auto robot{ std::make_unique<Robot>() };

    try {
        robot->init();

        io.run_one();

        std::cout << "Stopping..." << std::endl;

    }
    catch (std::exception const &e) {
        std::cerr << boost::diagnostic_information(e) << std::endl;
    }

    std::cout << "Cleanup..." << std::endl;
    robot->cleanup();
    std::cout << "Done" << std::endl;

    return 0;
}