#define BOOST_TEST_MODULE ASyncSignal
#include <boost/test/included/unit_test.hpp>

#include <memory>
#include <thread>
#include <chrono>
#include <boost/log/trivial.hpp>

#include <common/asyncsignal.h>

using namespace std::literals;



BOOST_AUTO_TEST_SUITE(asyncsignal_suite)



BOOST_AUTO_TEST_CASE(Test)
{
    using boost::asio::io_context;

    io_context io;

    auto signal_ptr { std::make_shared<Robot::ASyncSignal>(io) };
    uint64_t count { 0 };
    uint64_t calls { 0 };

    signal_ptr->connect([&count, &calls](auto cnt) {
        calls++;
        count += cnt;
    });
    signal_ptr->async_wait();

    auto &signal { *signal_ptr };

    signal();
    signal();
    signal();
    signal();
    io.run_for(10ms);

    // We should receive all 4 signals in one call
    BOOST_CHECK_EQUAL(calls, 1);
    BOOST_CHECK_EQUAL(count, 4);

    signal();
    io.run_for(10ms);
    signal();
    io.run_for(10ms);
    signal();
    io.run_for(10ms);

    // Signal should have been received immediately 
    BOOST_CHECK_EQUAL(calls , 4);
    BOOST_CHECK_EQUAL(count , 7);
}

BOOST_AUTO_TEST_CASE(TestRate)
{
    using boost::asio::io_context;

    io_context io;

    auto signal_ptr { std::make_shared<Robot::ASyncSignal>(io, 1s) };
    uint64_t count { 0 };
    uint64_t calls { 0 };

    signal_ptr->connect([&count, &calls](auto cnt) {
        calls++;
        count += cnt;
    });
    signal_ptr->async_wait();

    auto &signal { *signal_ptr };

    signal();
    io.run_for(10ms);
    signal();
    io.run_for(10ms);
    signal();
    io.run_for(10ms);
    signal();
    io.run_for(10ms);

    // One second has not yet elapsed so no signal should have been sent
    BOOST_CHECK_EQUAL(calls , 0);
    BOOST_CHECK_EQUAL(count , 0);

    io.run_for(2s);

    // We should receive all 4 signals in one call
    BOOST_CHECK_EQUAL(calls , 1);
    BOOST_CHECK_EQUAL(count , 4);

    io.run_for(1s);
    signal();
    io.run_for(10ms);

    // Signal should have been received immediately because the it has been more than 1s since the last signal
    BOOST_CHECK_EQUAL(calls , 2);
    BOOST_CHECK_EQUAL(count , 5);
}


BOOST_AUTO_TEST_SUITE_END()

