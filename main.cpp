#include <memory>
#include <iostream>
#include <exception>
#include <boost/format.hpp>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/thread/thread.hpp>
#include <boost/exception/all.hpp>
#include <boost/bind.hpp>

#include <robot.h>
#include <led/control.h>
#include <rc/receiver.h>
#include <input/value.h>

void test_values() {
    using namespace std;
    using namespace Robot::Input;
    for (uint32_t v = Value::PULSE_MIN; v<=Value::PULSE_MAX; v++) {
        double percent_d = (double)(2*((std::int32_t)v-Value::PULSE_CENTER))/Value::PULSE_RANGE;
        float percent_f = (float)(2*((std::int32_t)v-Value::PULSE_CENTER))/Value::PULSE_RANGE;
        uint32_t fp_d = (uint32_t)((percent_d * Value::PULSE_RANGE) + Value::PULSE_MIN);
        uint32_t fp_f = (uint32_t)((percent_f * Value::PULSE_RANGE) + Value::PULSE_MIN);

        cout << 
            boost::format("%4d  %1.5f  %1.5f") % v % percent_d % percent_f
            << "  " << ( (fp_d!=fp_f)?"<<<<<":"")
         << endl;
    }
}

int main() {
    //test_values();

    std::cout << Robot::Input::Value::CENTER.asAngleRadians() << std::endl;
    std::cout << Robot::Config::WHEEL_DIAMETER_MM << std::endl;

    #if 0
    using boost::asio::io_context, boost::asio::signal_set;

    io_context io;

    signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(boost::bind(&io_context::stop, &io));

    auto robot{ std::make_unique<Robot::Robot>() };

    try {
        robot->init();

        sleep(1);

        std::cout << "Stopping..." << std::endl;

    }
    catch (std::exception const &e) {
        std::cerr << boost::diagnostic_information(e) << std::endl;
    }

    std::cout << "Cleanup..." << std::endl;
    robot->cleanup();
    std::cout << "Done" << std::endl;
    #endif

    return 0;
}