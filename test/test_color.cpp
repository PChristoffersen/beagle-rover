#define BOOST_TEST_MODULE Color
#include <boost/test/included/unit_test.hpp>

#include <thread>
#include <chrono>
#include <iostream>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

#include <robot.h>
#include <led/color.h>
#include <led/colorlayer.h>

using namespace std::literals;

static Robot::Logging::LogInit __log_init;

using Robot::LED::Color, Robot::LED::ColorArray, Robot::LED::RawColorArray;


static void CHECK_COLOR(const Color col1, const Color col2, std::string test_name)
{
    BOOST_LOG_TRIVIAL(info) << test_name << " Check " << col1 << "  ==  " << col2;
    BOOST_CHECK_MESSAGE(col1 == col2,             test_name << " Failed color compare " << col1 << " != " << col2);
    BOOST_CHECK_MESSAGE(col1.red() == col2.red(), test_name << " Failed red compare " << std::hex << std::setw(2) << static_cast<uint32_t>(col1.red()) << " != " << static_cast<uint32_t>(col2.red()));
    BOOST_CHECK_MESSAGE(col1.green() == col2.green(), test_name << " Failed green compare " << std::hex << std::setw(2) << static_cast<uint32_t>(col1.green()) << " != " << static_cast<uint32_t>(col2.green()));
    BOOST_CHECK_MESSAGE(col1.blue() == col2.blue(), test_name << " Failed blue compare " << std::hex << std::setw(2) << static_cast<uint32_t>(col1.blue()) << " != " << static_cast<uint32_t>(col2.blue()));
    BOOST_CHECK_MESSAGE(col1.alpha() == col2.alpha(), test_name << " Failed alpha compare " << std::hex << std::setw(2) << static_cast<uint32_t>(col1.alpha()) << " != " << static_cast<uint32_t>(col2.alpha()));
    BOOST_CHECK_MESSAGE(col1.raw() == col2.raw(), test_name << " Failed raw compare " << std::hex << std::setw(8) << col1.raw() << " != " << col2.raw());
}



BOOST_AUTO_TEST_SUITE(led_suite)

BOOST_AUTO_TEST_CASE(TestColor)
{
    CHECK_COLOR(Color { 0xFF, 0xFF, 0xFF }, Color::WHITE, "White");
    CHECK_COLOR(Color { 0xFF, 0xFF, 0xFF, 0xFF }, Color::WHITE, "WhiteAlpha");
    CHECK_COLOR(Color { 0x00, 0x00, 0x00 }, Color::BLACK, "Black");
    CHECK_COLOR(Color { 0x00, 0x00, 0x00, 0xFF }, Color::BLACK, "BlackAlpha");
    CHECK_COLOR(Color { 0xFF, 0x00, 0x00 }, Color::RED, "Red");
    CHECK_COLOR(Color { 0x00, 0xFF, 0x00 }, Color::GREEN, "Green");
    CHECK_COLOR(Color { 0x00, 0x00, 0xFF }, Color::BLUE, "Blue");
    CHECK_COLOR(Color{}, Color::TRANSPARENT, "Default");

    Color col { 0x11, 0x22, 0x33, 0x44 };
    CHECK_COLOR(col, Color { Color::rawRGBA(0x11, 0x22, 0x33, 0x44) }, "Constructor");

    Color col2 = Color::TRANSPARENT;
    CHECK_COLOR(col2, Color::TRANSPARENT, "Default");
    col2 = col;
    CHECK_COLOR(col, col2, "Assign");

    Color col3 { col };
    CHECK_COLOR(col, col3, "Copy");

    BOOST_CHECK_MESSAGE(col.raw() == Color::rawRGBA(0x11, 0x22, 0x33, 0x44), "Failed raw compare " << std::hex << std::setw(8) << col.raw() << " != " << Color::rawRGBA(0x11, 0x22, 0x33, 0x44));

}

BOOST_AUTO_TEST_CASE(TestBlend)
{
    Color col { Color::TRANSPARENT };

    col << Color::BLACK;
    CHECK_COLOR(col, Color::BLACK, "Blend Black");

    col = Color::TRANSPARENT;
    col << Color::BLACK << Color::WHITE << Color::RED << Color::BLUE;
    CHECK_COLOR(col, Color::BLUE, "Blend multiple opague");

    col = Color::BLACK;
    col << Color(0xFF, 0x00, 0x00, 0x80);
    CHECK_COLOR(col, Color { 0x80, 0x00, 0x00, 0xFF }, "Blend alpha");

    col = Color::BLACK;
    col << Color(0xFF, 0x00, 0x00, 0xFF) << Color(0x00, 0xFF, 0x00, 0x80);
    CHECK_COLOR(col, Color { 0x80, 0x80, 0x00, 0xFF }, "Blend alpha 2");

    col = Color::BLACK;
    col << Color(0x00, 0xFF, 0x00, 0xFF) << Color(0x00, 0x00, 0xFF, 0x80);
    CHECK_COLOR(col, Color { 0x00, 0x80, 0x80, 0xFF }, "Blend alpha 3");

    col = Color::BLACK;
    col << Color(0x00, 0x00, 0xFF, 0xFF) << Color(0xFF, 0x00, 0x00, 0x80);
    CHECK_COLOR(col, Color { 0x80, 0x00, 0x80, 0xFF }, "Blend alpha 4");

    col = Color::GREEN;
    col << Color(0x00,0x00,0x00,0x80);
    CHECK_COLOR(col, Color { 0x00, 0x80, 0x00, 0xFF }, "Blend alpha 5");

}

BOOST_AUTO_TEST_CASE(TestBrightness)
{
    for (Color source : std::initializer_list { Color::WHITE, Color::BLACK, Color::RED, Color::GREEN, Color::BLUE }) {
        Color c, c2, expected;
        Color::brightness_type b;

        c = source * 0.0f;
        c2 = source;
        c2*= 0.0f;
        expected = Color::BLACK;
        CHECK_COLOR(c, expected,  "Brightness *  0.0");
        CHECK_COLOR(c2, expected, "Brightness *= 0.0");

        b = 0.5f;
        c = source * b;
        c2 = source;
        c2*= b;
        expected = Color { static_cast<uint8_t>(source.red()?0x80:0x00), static_cast<uint8_t>(source.green()?0x80:0x00), static_cast<uint8_t>(source.blue()?0x80:0x00) };
        CHECK_COLOR(c, expected,  "Brightness *  0.5");
        CHECK_COLOR(c2, expected, "Brightness *= 0.5");
        CHECK_COLOR(c, source * b,  "Brightness2 *  0.5");
        CHECK_COLOR(c2, source * b, "Brightness2 *= 0.5");

        c = source * 1.0f;
        c2 = source;
        c2*= 1.0f;
        expected = source;
        CHECK_COLOR(c, expected,  "Brightness *  1.0");
        CHECK_COLOR(c2, expected, "Brightness *= 1.0");

        // Check that we round brightness correctly
        b = 254.5f/255.0f;
        c = source * b;
        c2 = source;
        c2*= b;
        expected = source;
        CHECK_COLOR(c, expected,  (boost::format("Brightness *  %.4f") % b).str());
        CHECK_COLOR(c2, expected, (boost::format("Brightness *= %.4f") % b).str());

        b = 0.4999f/255.0f;
        c = source * b;
        c2 = source;
        c2*= b;
        expected = Color::BLACK;
        CHECK_COLOR(c, expected,  (boost::format("Brightness *  %.4f") % b).str());
        CHECK_COLOR(c2, expected, (boost::format("Brightness *= %.4f") % b).str());

    }

}

BOOST_AUTO_TEST_CASE(TestCorrect)
{
    auto test_sources = std::initializer_list { Color::WHITE, Color::BLACK, Color::RED, Color::GREEN, Color::BLUE };
    auto test_correct = std::initializer_list { 
        Color::Correction::Typical8mmPixel,
        Color::Correction::TypicalLEDStrip,
        Color::Correction::TypicalPixelString,
        Color::Correction::TypicalSMD5050,
        Color::Correction::UncorrectedColor,
    };

    for (auto &source : test_sources) {
        for (auto &correct : test_correct) {
            Color c, c2, expected;

            auto correctRed   { Color::rawRed(static_cast<Color::raw_type>(correct)) };
            auto correctGreen { Color::rawGreen(static_cast<Color::raw_type>(correct)) };
            auto correctBlue  { Color::rawBlue(static_cast<Color::raw_type>(correct)) };

            c = source * correct;
            c2 = source;
            c2*= correct;
            expected = Color { 
                static_cast<uint8_t>((source.red()&&correctRed)?correctRed:0x00), 
                static_cast<uint8_t>((source.green()&&correctGreen)?correctGreen:0x00), 
                static_cast<uint8_t>((source.blue()&&correctBlue)?correctBlue:0x00) 
            };
            CHECK_COLOR(c, expected,  (boost::format("Correct *  (%+02x%+02x%+02x)") % static_cast<uint32_t>(correctRed) % static_cast<uint32_t>(correctGreen) % static_cast<uint32_t>(correctBlue)).str());
            CHECK_COLOR(c2, expected, (boost::format("Correct *= (%+02x%+02x%+02x)") % static_cast<uint32_t>(correctRed) % static_cast<uint32_t>(correctGreen) % static_cast<uint32_t>(correctBlue)).str());
        }
    }
}


BOOST_AUTO_TEST_CASE(TestColorArray)
{
    constexpr auto ARRAY_SIZE { 5u };
    std::array<Color, ARRAY_SIZE> test_colors { 
        Color::RED,
        Color::GREEN,
        Color::BLUE,
        Color::WHITE,
        Color::BLACK,
    };

    // Construct color array with test colors
    ColorArray<ARRAY_SIZE> colors;
    for (uint i=0u; i<colors.size(); i++) {
        colors[i] = test_colors[i];
    }

    auto colors2 { colors };
    auto colors3 { colors };

    // Verify array 
    for (uint i=0u; i<colors.size(); i++) {
        CHECK_COLOR(colors[i], test_colors[i], (boost::format("Check color[%u]") % i).str());
        CHECK_COLOR(colors2[i], test_colors[i], (boost::format("Check color2[%u]") % i).str());
        CHECK_COLOR(colors3[i], test_colors[i], (boost::format("Check color3[%u]") % i).str());
    }

    // Check brightness
    colors2 *= 0x80;
    for (uint i=0u; i<colors.size(); i++) {
        CHECK_COLOR(colors2[i], test_colors[i] * 0x80, (boost::format("Check brightness[%u]") % i).str());
    }

    // Check correction
    auto cor = Color::Correction::TypicalLEDStrip;
    colors3 *= cor;
    for (uint i=0u; i<colors.size(); i++) {
        CHECK_COLOR(colors3[i], test_colors[i] * cor, (boost::format("Check correction[%u]") % i).str());
    }


    // Test ColorArray to RawColorArray conversion
    RawColorArray<ARRAY_SIZE> raw_colors { colors };
    for (uint i=0u; i<colors.size(); i++) {
        BOOST_CHECK_MESSAGE(raw_colors[i] == colors[i].raw(), "Failed raw compare " << std::hex << std::setw(8) << raw_colors[i] << " != " << colors[i].raw());
    }

}

BOOST_AUTO_TEST_SUITE_END()

