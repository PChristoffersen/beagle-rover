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

    auto signal { std::make_shared<Robot::ASyncSignal>(io) };
    uint64_t count { 0 };
    uint64_t calls { 0 };

    signal->connect([&count, &calls](auto cnt) {
        calls++;
        count += cnt;
    });
    signal->async_wait();

    (*signal)();
    (*signal)();
    (*signal)();
    (*signal)();
    io.run_for(10ms);

    // We should receive all 4 signals in one call
    BOOST_TEST(calls == 1);
    BOOST_TEST(count == 4);

    (*signal)();
    io.run_for(10ms);
    (*signal)();
    io.run_for(10ms);
    (*signal)();
    io.run_for(10ms);

    // Signal should have been received immediately 
    BOOST_TEST(calls == 4);
    BOOST_TEST(count == 7);
}

BOOST_AUTO_TEST_CASE(TestRate)
{
    using boost::asio::io_context;

    io_context io;

    auto signal { std::make_shared<Robot::ASyncSignal>(io, 1s) };
    uint64_t count { 0 };
    uint64_t calls { 0 };

    signal->connect([&count, &calls](auto cnt) {
        calls++;
        count += cnt;
    });
    signal->async_wait();

    (*signal)();
    io.run_for(10ms);
    (*signal)();
    io.run_for(10ms);
    (*signal)();
    io.run_for(10ms);
    (*signal)();
    io.run_for(10ms);

    // One second has not yet elapsed so no signal should have been sent
    BOOST_TEST(calls == 0);
    BOOST_TEST(count == 0);

    io.run_for(2s);

    // We should receive all 4 signals in one call
    BOOST_TEST(calls == 1);
    BOOST_TEST(count == 4);

    io.run_for(1s);
    (*signal)();
    io.run_for(10ms);

    // Signal should have been received immediately because the it has been more than 1s since the last signal
    BOOST_TEST(calls == 2);
    BOOST_TEST(count == 5);
}


BOOST_AUTO_TEST_SUITE_END()

