#define BOOST_TEST_MODULE Subscription
#include <boost/test/included/unit_test.hpp>

#include <memory>
#include <thread>
#include <ostream>
#include <sstream>
#include <boost/asio.hpp>

#include <robotdebug.h>
#include <common/notifysubscription.h>

using namespace std::literals;

#ifdef ROBOT_DEBUG
using Robot::Debug::TestComponent;
using subscription_type = Robot::NotifySubscription<TestComponent::notify_type>;

static std::string res2str(const subscription_type::result_type &res) 
{
    std::stringstream s;
    std::string sep;
    s << "(";
    for (auto &v : res) {
        s << sep << v;
        sep = ",";
    }
    s << ")";
    return s.str();
}

#endif


BOOST_AUTO_TEST_SUITE(subscription_suite)

#ifdef ROBOT_DEBUG

BOOST_AUTO_TEST_CASE(TestSubscription)
{
    auto component { std::make_shared<Robot::Debug::TestComponent>() };


    auto sub { notify_subscribe(*component) };

    {
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res.empty(), "Expected empty set "<< res2str(res));
    }

    component->ping(0);

    {
        auto expected { subscription_type::result_type { std::initializer_list { 0, } } };
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res == expected, "Subscription read=" << res2str(res) << " expected=" << res2str(expected));
    }

}


BOOST_AUTO_TEST_CASE(TestSubscriptionFiltered)
{
    auto component { std::make_shared<Robot::Debug::TestComponent>() };

    auto sub { notify_subscribe(*component, std::initializer_list { 10, 11, 12 } ) };

    {
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res.empty(), "Expected empty set "<< res2str(res));
    }

    component->ping(0);
    {
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res.empty(), "Expected empty set "<< res2str(res));
    }


    component->ping(10);
    {
        auto expected { subscription_type::result_type { std::initializer_list { 10, } } };
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res == expected, "Subscription read=" << res2str(res) << " expected=" << res2str(expected));
    }

    for (auto i=0; i<100; i++) {
        component->ping(i);
    }
    {
        auto expected { subscription_type::result_type { std::initializer_list { 10, 11, 12 } } };
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res == expected, "Subscription read=" << res2str(res) << " expected=" << res2str(expected));
    }

}



BOOST_AUTO_TEST_CASE(TestSubscriptionAttach)
{
    auto component { std::make_shared<Robot::Debug::TestComponent>() };
    auto component2 { std::make_shared<Robot::Debug::TestComponent>() };

    constexpr auto offset { 100 };
    auto sub { notify_subscribe(*component) };
    notify_attach(sub, *component2, offset);

    {
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res.empty(), "Expected empty set " << res2str(res));
    }

    component->ping(0);
    component2->ping(0);
    {
        subscription_type::result_type expected { std::initializer_list { 0, 0+offset } };
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res == expected, "Subscription read=" << res2str(res) << " expected=" << res2str(expected));
    }

    component->ping(0);
    {
        subscription_type::result_type expected { std::initializer_list { 0 } };
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res == expected, "Subscription read=" << res2str(res) << " expected=" << res2str(expected));
    }


    component2->ping(0);
    {
        subscription_type::result_type expected { std::initializer_list { 0+offset } };
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res == expected, "Subscription read=" << res2str(res) << " expected=" << res2str(expected));
    }


    {
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res.empty(), "Expected empty set " << res2str(res));
    }
}


BOOST_AUTO_TEST_CASE(TestSubscriptionAttachFiltered)
{
    auto component { std::make_shared<Robot::Debug::TestComponent>() };
    auto component2 { std::make_shared<Robot::Debug::TestComponent>() };

    constexpr auto offset { 100 };
    auto sub { notify_subscribe(*component, std::initializer_list { 10, 11, 12 }) };
    notify_attach(sub, *component2, offset, std::initializer_list { 10, 20, 21 } );

    {
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res.empty(), "Expected empty set "<< res2str(res));
    }

    component->ping(0);
    component2->ping(0);
    {
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res.empty(), "Expected empty set "<< res2str(res));
    }


    component->ping(10);
    component2->ping(10);
    {
        auto expected { subscription_type::result_type { std::initializer_list { 10, 10+offset } } };
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res == expected, "Subscription read=" << res2str(res) << " expected=" << res2str(expected));
    }

    component->ping(11);
    component2->ping(11);
    {
        auto expected { subscription_type::result_type { std::initializer_list { 11 } } };
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res == expected, "Subscription read=" << res2str(res) << " expected=" << res2str(expected));
    }

    component->ping(20);
    component2->ping(20);
    {
        auto expected { subscription_type::result_type { std::initializer_list { 20+offset } } };
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res == expected, "Subscription read=" << res2str(res) << " expected=" << res2str(expected));
    }


    for (auto i=0; i<100; i++) {
        component->ping(i);
        component2->ping(i);
    }
    {
        auto expected { subscription_type::result_type { std::initializer_list { 10, 11, 12, 10+offset, 20+offset, 21+offset } } };
        auto res { sub->read() };
        BOOST_CHECK_MESSAGE(res == expected, "Subscription read=" << res2str(res) << " expected=" << res2str(expected));
    }
}


#endif

BOOST_AUTO_TEST_SUITE_END()

