#define BOOST_TEST_MODULE LEDS
#include <boost/test/included/unit_test.hpp>

#include <thread>
#include <chrono>

#include <robot.h>
#include <led/control.h>

using namespace std::literals;

static Robot::Logging::LogInit __log_init;

static constexpr std::initializer_list ALL_ANIMATIONS { 
    Robot::LED::AnimationMode::NONE, 
    Robot::LED::AnimationMode::HEADLIGHTS,
    Robot::LED::AnimationMode::CONSTRUCTION,
    Robot::LED::AnimationMode::POLICE,
    Robot::LED::AnimationMode::AMBULANCE,
    Robot::LED::AnimationMode::KNIGHT_RIDER,
    Robot::LED::AnimationMode::RAINBOW
};


static constexpr std::initializer_list ALL_INDICATORS { 
    Robot::LED::IndicatorMode::NONE,
    Robot::LED::IndicatorMode::LEFT,
    Robot::LED::IndicatorMode::RIGHT,
    Robot::LED::IndicatorMode::HAZARD
};

BOOST_AUTO_TEST_SUITE(led_suite)

BOOST_AUTO_TEST_CASE(TestAnimation)
{
    auto robot { std::make_unique<Robot::Robot>() };
    robot->init();
    {
        auto control = robot->ledControl();
        
        for (auto anim : ALL_ANIMATIONS) {
            control->setAnimation(anim);
            std::this_thread::sleep_for(2s);
        }
        control->setAnimation(Robot::LED::AnimationMode::NONE);
    }
    robot->cleanup();
}


BOOST_AUTO_TEST_CASE(TestIndicators)
{
    auto robot { std::make_unique<Robot::Robot>() };
    robot->init();
    {
        auto control = robot->ledControl();
        
        for (auto indicator : ALL_INDICATORS) {
            control->setIndicators(indicator);
            std::this_thread::sleep_for(2s);
        }
        control->setIndicators(Robot::LED::IndicatorMode::NONE);
    }
    robot->cleanup();
}


BOOST_AUTO_TEST_CASE(TestRapid)
{
    auto robot { std::make_unique<Robot::Robot>() };
    robot->init();
    {
        auto control = robot->ledControl();
        
        for (auto anim : ALL_ANIMATIONS) {
            control->setAnimation(anim);
            for (auto indicator : ALL_INDICATORS) {
                control->setIndicators(indicator);
                std::this_thread::sleep_for(50ms);
            }
        }
    }
    robot->cleanup();
}


BOOST_AUTO_TEST_SUITE_END()

