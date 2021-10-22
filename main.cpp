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

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::chrono;



int main() {
    io_context io;


    signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(bind(&io_context::stop, &io));

    boost::shared_ptr<Robot> robot(new Robot);

    try {
        robot->init();

        robot->kinematic()->setDriveMode(Kinematic::DRIVE_BALANCING);

        io.run();

        std::cout << "Stopping..." << std::endl;

    }
    catch (std::exception const &e) {
        cerr << diagnostic_information(e) << endl;
    }

    std::cout << "Cleanup..." << std::endl;
    robot->cleanup();
    std::cout << "Done" << std::endl;

    return 0;
}