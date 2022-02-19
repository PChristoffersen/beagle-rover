#define BOOST_TEST_MODULE Value
#include <boost/test/included/unit_test.hpp>

#include <thread>
#include <chrono>

#include <common/value.h>

using namespace std::literals;



BOOST_AUTO_TEST_SUITE(value_suite)

BOOST_AUTO_TEST_CASE(Test)
{
    using namespace Robot;

    BOOST_CHECK_EQUAL(Value::CENTER.asServoPulse(), Value::PULSE_CENTER);
    BOOST_CHECK_EQUAL(Value::MIN.asServoPulse(), Value::PULSE_MIN);
    BOOST_CHECK_EQUAL(Value::MAX.asServoPulse(), Value::PULSE_MAX);

    
}


BOOST_AUTO_TEST_CASE(TestPulse)
{
    using namespace Robot;

    Value v;

    v = Value::PULSE_MIN;
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MIN);
    BOOST_CHECK_EQUAL(Value::PULSE_MIN, (Value::value_type)v);

    v = Value::PULSE_CENTER;
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_CENTER);
    BOOST_CHECK_EQUAL(Value::PULSE_CENTER, (Value::value_type)v);

    v = Value::PULSE_MAX;
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MAX);
    BOOST_CHECK_EQUAL(Value::PULSE_MAX, (Value::value_type)v);

    v = 0;
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MIN);
    BOOST_CHECK_EQUAL(Value::PULSE_MIN, (Value::value_type)v);

    v = 999999;
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MAX);
    BOOST_CHECK_EQUAL(Value::PULSE_MAX, (Value::value_type)v);
}


BOOST_AUTO_TEST_CASE(TestFloat)
{
    using namespace Robot;

    Value v;

    v = Value::fromFloat(-1.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MIN);
    BOOST_CHECK_EQUAL(v.asFloat(), -1.0f);
    BOOST_CHECK_EQUAL(-1.0f, (float)v);

    v = Value::fromFloat(0.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_CENTER);
    BOOST_CHECK_EQUAL(v.asFloat(), 0.0f);
    BOOST_CHECK_EQUAL(0.0f, (float)v);

    v = Value::fromFloat(1.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MAX);
    BOOST_CHECK_EQUAL(v.asFloat(), 1.0f);
    BOOST_CHECK_EQUAL(1.0f, (float)v);

    v = Value::fromFloat(-9999.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MIN);
    BOOST_CHECK_EQUAL(v.asFloat(), -1.0f);
    BOOST_CHECK_EQUAL(-1.0f, (float)v);

    v = Value::fromFloat(9999.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MAX);
    BOOST_CHECK_EQUAL(v.asFloat(), 1.0f);
    BOOST_CHECK_EQUAL(1.0f, (float)v);
}


BOOST_AUTO_TEST_CASE(TestPercent)
{
    using namespace Robot;

    Value v;

    v = Value::fromPercent(0.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MIN);
    BOOST_CHECK_EQUAL(v.asPercent(), 0.0f);

    v = Value::fromPercent(0.5f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_CENTER);
    BOOST_CHECK_EQUAL(v.asPercent(), 0.5f);

    v = Value::fromPercent(1.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MAX);
    BOOST_CHECK_EQUAL(v.asPercent(), 1.0f);

    v = Value::fromPercent(9999.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MAX);
    BOOST_CHECK_EQUAL(v.asPercent(), 1.0f);

    v = Value::fromPercent(-9999.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MIN);
    BOOST_CHECK_EQUAL(v.asPercent(), 0.0f);

}



BOOST_AUTO_TEST_CASE(TestAngle)
{
    using namespace Robot;

    Value v;

    v = Value::fromAngle(-M_PI_2);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MIN);
    BOOST_CHECK_EQUAL(v.asAngle(), -(float)M_PI_2);

    v = Value::fromAngleDegrees(-90.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MIN);
    BOOST_CHECK_EQUAL(v.asAngleDegrees(), -90.0f);

    v = Value::fromAngle(0.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_CENTER);
    BOOST_CHECK_EQUAL(v.asAngle(), 0.0f);

    v = Value::fromAngleDegrees(0.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_CENTER);
    BOOST_CHECK_EQUAL(v.asAngleDegrees(), 0.0f);

    v = Value::fromAngle(M_PI_2);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MAX);
    BOOST_CHECK_EQUAL(v.asAngle(), (float)M_PI_2);

    v = Value::fromAngleDegrees(90.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MAX);
    BOOST_CHECK_EQUAL(v.asAngleDegrees(), 90.0f);


    v = Value::fromAngle(-9999.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MIN);
    BOOST_CHECK_EQUAL(v.asAngle(), -(float)M_PI_2);

    v = Value::fromAngleDegrees(-9999.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MIN);
    BOOST_CHECK_EQUAL(v.asAngleDegrees(), -90.0f);

    v = Value::fromAngle(9999.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MAX);
    BOOST_CHECK_EQUAL(v.asAngle(), (float)M_PI_2);

    v = Value::fromAngleDegrees(9999.0f);
    BOOST_CHECK_EQUAL(v.asServoPulse(), Value::PULSE_MAX);
    BOOST_CHECK_EQUAL(v.asAngleDegrees(), 90.0f);
}


BOOST_AUTO_TEST_SUITE_END()

