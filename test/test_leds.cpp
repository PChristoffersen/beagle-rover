#define BOOST_TEST_MODULE LEDS
#include <boost/test/included/unit_test.hpp>

#include <thread>
#include <chrono>
#include <boost/log/trivial.hpp>

#include <robot.h>
#include <common/notifysubscription.h>
#include <led/control.h>

using namespace std::literals;

static Robot::Logging::LogInit __log_init;

struct animation_entry {
    Robot::LED::AnimationMode anim;
    uint num_shows;
};

static constexpr std::initializer_list ALL_ANIMATIONS { 
    animation_entry { Robot::LED::AnimationMode::HEADLIGHTS, 0 },
    animation_entry { Robot::LED::AnimationMode::CONSTRUCTION, 3 },
    animation_entry { Robot::LED::AnimationMode::POLICE, 3 },
    animation_entry { Robot::LED::AnimationMode::AMBULANCE, 3 },
    animation_entry { Robot::LED::AnimationMode::KNIGHT_RIDER, 9 },
    animation_entry { Robot::LED::AnimationMode::RAINBOW, 5 },
};


static constexpr std::initializer_list ALL_INDICATORS { 
    Robot::LED::IndicatorMode::LEFT,
    Robot::LED::IndicatorMode::RIGHT,
    Robot::LED::IndicatorMode::HAZARD
};

static void wait_for_show(std::shared_ptr<Robot::NotifySubscription<Robot::LED::Control::NotifyType>> sub, std::chrono::steady_clock::duration timeout, bool fail_on_timeout=true) {
    using clock = std::chrono::steady_clock;

    auto start { clock::now() };
    while (clock::now()-start < timeout) {
        auto remaining_timeout { timeout - (clock::now()-start) };
        auto res { sub->read(std::chrono::duration_cast<std::chrono::milliseconds>(remaining_timeout)) };
        if (res.contains(Robot::LED::Control::NOTIFY_UPDATE)) {
            return;
        }
    }
    if (fail_on_timeout) {
        BOOST_FAIL("Timeout while waiting for show");
    }
}


BOOST_AUTO_TEST_SUITE(led_suite)


BOOST_AUTO_TEST_CASE(TestAnimation)
{
    auto robot { std::make_unique<Robot::Robot>() };
    robot->init();
    {
        auto control { robot->ledControl() };
        auto sub { Robot::notify_subscribe(*control) };
        
        // Make sure we don't have any animations running
        control->setAnimation(Robot::LED::AnimationMode::NONE);
        control->setIndicators(Robot::LED::IndicatorMode::NONE);
        wait_for_show(sub, 250ms, false);
        sub->clear();

        for (auto entry : ALL_ANIMATIONS) {
            BOOST_TEST_MESSAGE("Testing animation "<< static_cast<std::underlying_type<Robot::LED::AnimationMode>::type>(entry.anim));
            sub->clear();
            control->setAnimation(entry.anim);
            wait_for_show(sub, 100ms); // Initial update
            for (auto i=0u; i<entry.num_shows; i++) {
                wait_for_show(sub, 2s); // Timer update
            }
        }
        control->setAnimation(Robot::LED::AnimationMode::NONE);
        wait_for_show(sub, 250ms);
    }
    robot->cleanup();
}

BOOST_AUTO_TEST_CASE(TestIndicators)
{
    auto robot { std::make_unique<Robot::Robot>() };
    robot->init();
    {
        auto control = robot->ledControl();
        auto sub { Robot::notify_subscribe(*control) };
        
        // Make sure we don't have any animations running
        control->setAnimation(Robot::LED::AnimationMode::NONE);
        control->setIndicators(Robot::LED::IndicatorMode::NONE);
        wait_for_show(sub, 250ms, false);
        sub->clear();
        
        for (auto indicator : ALL_INDICATORS) {
            BOOST_TEST_MESSAGE("Testing indicator "<< static_cast<std::underlying_type<Robot::LED::IndicatorMode>::type>(indicator));
            sub->clear();
            control->setIndicators(indicator);
            for (auto i=0u; i<3; i++) {
                wait_for_show(sub, 2s); // Timer update
            }
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
        
        for (auto &entry : ALL_ANIMATIONS) {
            control->setAnimation(entry.anim);
            for (auto indicator : ALL_INDICATORS) {
                control->setIndicators(indicator);
                std::this_thread::sleep_for(10ms);
            }
        }
    }
    robot->cleanup();
}



BOOST_AUTO_TEST_CASE(TestLayers)
{
    using namespace Robot::LED;

    auto robot { std::make_unique<Robot::Robot>() };
    robot->init();
    {
        auto control = robot->ledControl();
        auto layer1 { std::make_shared<ColorLayer>("Layer1", 15) };
        auto layer2 { std::make_shared<ColorLayer>("Layer2", 16) };
        auto sub { Robot::notify_subscribe(*control) };

        layer1->setVisible(true);
        layer2->setVisible(true);

        wait_for_show(sub, 200ms, false);
        control->setBackground(Color::BLACK);
        control->attachLayer(layer1);
        wait_for_show(sub, 2s);

        BOOST_TEST_MESSAGE("Updating layer 1");
        {
            layer1->mutex_lock();
            (*layer1)[0] = Color::RED;
            (*layer1)[1] = Color::GREEN;
            (*layer1)[2] = Color::BLUE;
            layer1->update();
            layer1->mutex_unlock();
            wait_for_show(sub, 500ms);

            auto pixels { control->pixels() };
            BOOST_CHECK_MESSAGE(pixels[0] == Color::RED.data(), "Expected pixel 0 to be RED");
            BOOST_CHECK_MESSAGE(pixels[1] == Color::GREEN.data(), "Expected pixel 1 to be GREEN");
            BOOST_CHECK_MESSAGE(pixels[2] == Color::BLUE.data(), "Expected pixel 2 to be BLUE");
            BOOST_CHECK_MESSAGE(pixels[3] == Color::BLACK.data(), "Expected pixel 3 to be BLACK");
        }

        BOOST_TEST_MESSAGE("Hide layer 1");
        {
            layer1->setVisible(false);
            wait_for_show(sub, 500ms);

            auto pixels { control->pixels() };
            BOOST_CHECK_MESSAGE(pixels[0] == Color::BLACK.data(), "Expected pixel 0 to be BLACK");
            BOOST_CHECK_MESSAGE(pixels[1] == Color::BLACK.data(), "Expected pixel 1 to be BLACK");
            BOOST_CHECK_MESSAGE(pixels[2] == Color::BLACK.data(), "Expected pixel 2 to be BLACK");
            BOOST_CHECK_MESSAGE(pixels[3] == Color::BLACK.data(), "Expected pixel 3 to be BLACK");
        }

        BOOST_TEST_MESSAGE("Show layer 1");
        {
            layer1->setVisible(true);
            wait_for_show(sub, 500ms);

            auto pixels { control->pixels() };
            BOOST_CHECK_MESSAGE(pixels[0] == Color::RED.data(), "Expected pixel 0 to be RED");
            BOOST_CHECK_MESSAGE(pixels[1] == Color::GREEN.data(), "Expected pixel 1 to be GREEN");
            BOOST_CHECK_MESSAGE(pixels[2] == Color::BLUE.data(), "Expected pixel 2 to be BLUE");
            BOOST_CHECK_MESSAGE(pixels[3] == Color::BLACK.data(), "Expected pixel 3 to be BLACK");
        }

        BOOST_TEST_MESSAGE("Change background");
        {
            Color bgcolor { 0x20, 0x30, 0x40 };
            control->setBackground(bgcolor);
            wait_for_show(sub, 500ms);

            auto pixels { control->pixels() };
            BOOST_CHECK_MESSAGE(pixels[0] == Color::RED.data(), "Expected pixel 0 to be RED");
            BOOST_CHECK_MESSAGE(pixels[1] == Color::GREEN.data(), "Expected pixel 1 to be GREEN");
            BOOST_CHECK_MESSAGE(pixels[2] == Color::BLUE.data(), "Expected pixel 2 to be BLUE");
            BOOST_CHECK_MESSAGE(pixels[3] == bgcolor.data(), "Expected pixel 3 to be Background color");
        }

        BOOST_TEST_MESSAGE("Change background");
        {
            control->setBackground(Color::BLACK);
            wait_for_show(sub, 500ms);

            auto pixels { control->pixels() };
            BOOST_CHECK_MESSAGE(pixels[0] == Color::RED.data(), "Expected pixel 0 to be RED");
            BOOST_CHECK_MESSAGE(pixels[1] == Color::GREEN.data(), "Expected pixel 1 to be GREEN");
            BOOST_CHECK_MESSAGE(pixels[2] == Color::BLUE.data(), "Expected pixel 2 to be BLUE");
            BOOST_CHECK_MESSAGE(pixels[3] == Color::BLACK.data(), "Expected pixel 3 to be BLACK");
        }


        BOOST_TEST_MESSAGE("Attach layer 2");
        {
            control->attachLayer(layer2);
            wait_for_show(sub, 500ms);

            auto pixels { control->pixels() };
            BOOST_CHECK_MESSAGE(pixels[0] == Color::RED.data(), "Expected pixel 0 to be RED");
            BOOST_CHECK_MESSAGE(pixels[1] == Color::GREEN.data(), "Expected pixel 1 to be GREEN");
            BOOST_CHECK_MESSAGE(pixels[2] == Color::BLUE.data(), "Expected pixel 2 to be BLUE");
            BOOST_CHECK_MESSAGE(pixels[3] == Color::BLACK.data(), "Expected pixel 3 to be BLACK");
        }

        BOOST_TEST_MESSAGE("Update layer 2");
        {
            layer2->mutex_lock();
            (*layer2)[0] = Color::BLUE;
            (*layer2)[1] = Color::RED;
            (*layer2)[2] = Color::GREEN;
            layer2->update();
            layer2->mutex_unlock();
            wait_for_show(sub, 500ms);

            auto pixels { control->pixels() };
            BOOST_CHECK_MESSAGE(pixels[0] == Color::BLUE.data(), "Expected pixel 0 to be BLUE");
            BOOST_CHECK_MESSAGE(pixels[1] == Color::RED.data(), "Expected pixel 1 to be RED");
            BOOST_CHECK_MESSAGE(pixels[2] == Color::GREEN.data(), "Expected pixel 2 to be GREEN");
            BOOST_CHECK_MESSAGE(pixels[3] == Color::BLACK.data(), "Expected pixel 3 to be BLACK");
        }

        BOOST_TEST_MESSAGE("Update layer 2 alpha");
        {
            layer1->mutex_lock();
            layer2->mutex_lock();
            (*layer1)[0] = Color::RED;
            (*layer1)[1] = Color::GREEN;
            (*layer1)[2] = Color::BLUE;
            (*layer2)[0] = Color { 0x00, 0xFF, 0x00, 0x80 };
            (*layer2)[1] = Color { 0x00, 0x00, 0xFF, 0x80 };
            (*layer2)[2] = Color { 0xFF, 0x00, 0x00, 0x80 };;
            control->update();
            layer2->mutex_unlock();
            layer1->mutex_unlock();
            wait_for_show(sub, 500ms);

            auto pixels { control->pixels() };
            BOOST_CHECK_MESSAGE(pixels[0] == Color(0x7f, 0x80, 0x00, 0xFF).data(), "Unexpected pixel 0: " << Color { pixels[0] });
            BOOST_CHECK_MESSAGE(pixels[1] == Color(0x00, 0x7f, 0x80, 0xFF).data(), "Unexpected pixel 1: " << Color { pixels[1] });
            BOOST_CHECK_MESSAGE(pixels[2] == Color(0x80, 0x00, 0x7f, 0xFF).data(), "Unexpected pixel 2: " << Color { pixels[2] });
            BOOST_CHECK_MESSAGE(pixels[3] == Color::BLACK.data(),                  "Unexpected pixel 3: " << Color { pixels[3] });
        }


        control->detachLayer(layer2);
        control->detachLayer(layer1);
    }
    robot->cleanup();
}

BOOST_AUTO_TEST_SUITE_END()

