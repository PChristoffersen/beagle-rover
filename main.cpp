#include <iostream>
#include <exception>
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/thread/thread.hpp>
#include <boost/exception/all.hpp>
#include <boost/bind.hpp>

#include "robot.h"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::chrono;

#if 0
class printer {
public:
  printer(boost::asio::io_context& io)
    : strand_(io),
      timer1_(io, boost::asio::chrono::seconds(1)),
      timer2_(io, boost::asio::chrono::seconds(1)),
      count_(0)
  {
    timer1_.async_wait(bind_executor(strand_, boost::bind(&printer::print1, this)));
    timer2_.async_wait(bind_executor(strand_, boost::bind(&printer::print2, this)));
  }

  ~printer()
  {
    std::cout << "Final count is " << count_ << std::endl;
  }

  void print1() {
    if (count_ < 10) {
      std::cout << "Timer 1: " << count_ << std::endl;
      ++count_;

      timer1_.expires_at(timer1_.expiry() + chrono::seconds(1));
      timer1_.async_wait(bind_executor(strand_, boost::bind(&printer::print1, this)));
    }
  }

  void print2() {
    if (count_ < 10) {
      std::cout << "Timer 2: " << count_ << std::endl;
      ++count_;

      timer2_.expires_at(timer2_.expiry() + chrono::seconds(1));
      timer2_.async_wait(bind_executor(strand_, boost::bind(&printer::print2, this)));
    }
  }

private:
    io_context::strand strand_;
    steady_timer timer1_;
    steady_timer timer2_;
    int count_;
};

#endif


int main() {
    io_context io;


    signal_set signals(io, SIGINT, SIGTERM);
    signals.async_wait(bind(&io_context::stop, &io));

    Robot robot;

    try {
        robot.init();


        boost::thread t(boost::bind(&io_context::run, &io));
        #if 0
        io.run();
        t.join();
        #else
        io.run();
        #endif

        std::cout << "Stopping..." << std::endl;

    }
    catch (std::exception const &e) {
        cerr << diagnostic_information(e) << endl;
    }

    std::cout << "Cleanup..." << std::endl;
    robot.cleanup();
    std::cout << "Done" << std::endl;

    return 0;
}