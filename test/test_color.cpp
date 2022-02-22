#define BOOST_TEST_MODULE Color
#include <boost/test/included/unit_test.hpp>

#include <thread>
#include <chrono>
#include <iostream>
#include <limits>
#include <boost/log/trivial.hpp>
#include <boost/format.hpp>

#include <robotlogging.h>
#include <led/color.h>
#include <led/colorlayer.h>

using namespace std::literals;

static Robot::Logging::LogInit __log_init;

using Robot::LED::Color, Robot::LED::ColorArray, Robot::LED::RawColorArray;


BOOST_AUTO_TEST_SUITE(led_suite)

BOOST_AUTO_TEST_CASE(TestColorSize)
{
    BOOST_LOG_TRIVIAL(info) << "Color size: " << sizeof(Color);
    BOOST_CHECK_EQUAL(sizeof(Color), 4);
}

BOOST_AUTO_TEST_CASE(TestColor)
{
    BOOST_CHECK_EQUAL(Color(0xFF, 0xFF, 0xFF), Color::WHITE);
    BOOST_CHECK_EQUAL(Color(0xFF, 0xFF, 0xFF, 0xFF), Color::WHITE);
    BOOST_CHECK_EQUAL(Color(0x00, 0x00, 0x00), Color::BLACK);
    BOOST_CHECK_EQUAL(Color(0x00, 0x00, 0x00, 0xFF), Color::BLACK);
    BOOST_CHECK_EQUAL(Color(0xFF, 0x00, 0x00), Color::RED);
    BOOST_CHECK_EQUAL(Color(0x00, 0xFF, 0x00), Color::GREEN);
    BOOST_CHECK_EQUAL(Color(0x00, 0x00, 0xFF), Color::BLUE);
    BOOST_CHECK_EQUAL(Color(), Color::TRANSPARENT);

    Color col { 0x11, 0x22, 0x33, 0x44 };
    BOOST_CHECK_EQUAL(col, Color(Color::rawRGBA(0x11, 0x22, 0x33, 0x44)));

    Color col2 = Color::TRANSPARENT;
    BOOST_CHECK_EQUAL(col2, Color::TRANSPARENT);
    col2 = col;
    BOOST_CHECK_EQUAL(col, col2);

    Color col3 { col };
    BOOST_CHECK_EQUAL(col, col3);

    BOOST_CHECK_EQUAL(col.raw(), Color::rawRGBA(0x11, 0x22, 0x33, 0x44));

}


BOOST_AUTO_TEST_CASE(TestFloatColor)
{
    BOOST_CHECK_EQUAL(Color(0.0f, 0.0f, 0.0f), Color(0x00, 0x00, 0x00));
    BOOST_CHECK_EQUAL(Color(0.0f, 0.0f, 0.0f, 0.0f), Color(0x00, 0x00, 0x00, 0x00));

    BOOST_CHECK_EQUAL(Color(1.0f, 1.0f, 1.0f), Color(0xFF, 0xFF, 0xFF));
    BOOST_CHECK_EQUAL(Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0xFF, 0xFF, 0xFF, 0xFF));

    BOOST_CHECK_EQUAL(Color(0.5f, 0.0f, 0.0f), Color(0x80, 0x00, 0x00));
    BOOST_CHECK_EQUAL(Color(0.0f, 0.5f, 0.0f), Color(0x00, 0x80, 0x00));
    BOOST_CHECK_EQUAL(Color(0.0f, 0.0f, 0.5f), Color(0x00, 0x00, 0x80));
    BOOST_CHECK_EQUAL(Color(0.0f, 0.0f, 0.0f, 0.5f), Color(0x00, 0x00, 0x00, 0x80));

    BOOST_CHECK_EQUAL(Color(254.5f/255.0f, 0.4999f/255.0f, 0.0f), Color(0xFF, 0x00, 0x00));
    BOOST_CHECK_EQUAL(Color(254.4999f/255.0f, 0.5f/255.0f, 0.0f), Color(0xFE, 0x01, 0x00));

    BOOST_CHECK_EQUAL(Color(-1.0f, 2.0f, 0.0f), Color(0x00, 0xFF, 0x00));
    BOOST_CHECK_EQUAL(Color(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()), Color(0xFF, 0xFF, 0xFF));
    BOOST_CHECK_EQUAL(Color(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()), Color(0x00, 0x00, 0x00));

}

BOOST_AUTO_TEST_CASE(TestIntColor)
{
    BOOST_CHECK_EQUAL(Color(0, 0, 0), Color(0x00, 0x00, 0x00));
    BOOST_CHECK_EQUAL(Color(0, 0, 0, 0), Color(0x00, 0x00, 0x00, 0x00));

    BOOST_CHECK_EQUAL(Color(255, 255, 255), Color(0xFF, 0xFF, 0xFF));
    BOOST_CHECK_EQUAL(Color(255, 255, 255, 255), Color(0xFF, 0xFF, 0xFF, 0xFF));

    BOOST_CHECK_EQUAL(Color(128, 0, 0), Color(0x80, 0x00, 0x00));
    BOOST_CHECK_EQUAL(Color(0, 128, 0), Color(0x00, 0x80, 0x00));
    BOOST_CHECK_EQUAL(Color(0, 0, 128), Color(0x00, 0x00, 0x80));
    BOOST_CHECK_EQUAL(Color(0, 0, 0, 128), Color(0x00, 0x00, 0x00, 0x80));

    BOOST_CHECK_EQUAL(Color(-1, 256, 0), Color(0x00, 0xFF, 0x00));
    BOOST_CHECK_EQUAL(Color(std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max()), Color(0xFF, 0xFF, 0xFF));
    BOOST_CHECK_EQUAL(Color(std::numeric_limits<int>::min(), std::numeric_limits<int>::min(), std::numeric_limits<int>::min()), Color(0x00, 0x00, 0x00));
}

BOOST_AUTO_TEST_CASE(TestUnsignedIntColor)
{
    BOOST_CHECK_EQUAL(Color(0u, 0u, 0u), Color(0x00, 0x00, 0x00));
    BOOST_CHECK_EQUAL(Color(0u, 0u, 0u, 0u), Color(0x00, 0x00, 0x00, 0x00));

    BOOST_CHECK_EQUAL(Color(255u, 255u, 255u), Color(0xFF, 0xFF, 0xFF));
    BOOST_CHECK_EQUAL(Color(255u, 255u, 255u, 255u), Color(0xFF, 0xFF, 0xFF, 0xFF));

    BOOST_CHECK_EQUAL(Color(128u, 0u, 0u), Color(0x80, 0x00, 0x00));
    BOOST_CHECK_EQUAL(Color(0u, 128u, 0u), Color(0x00, 0x80, 0x00));
    BOOST_CHECK_EQUAL(Color(0u, 0u, 128u), Color(0x00, 0x00, 0x80));
    BOOST_CHECK_EQUAL(Color(0u, 0u, 0u, 128u), Color(0x00, 0x00, 0x00, 0x80));

    BOOST_CHECK_EQUAL(Color(256u, 0u, 0u), Color(0xFF, 0x00, 0x00));
    BOOST_CHECK_EQUAL(Color(std::numeric_limits<unsigned int>::max(), std::numeric_limits<unsigned int>::max(), std::numeric_limits<unsigned int>::max()), Color(0xFF, 0xFF, 0xFF));
    BOOST_CHECK_EQUAL(Color(std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::min()), Color(0x00, 0x00, 0x00));
}


BOOST_AUTO_TEST_CASE(TestStringColorInvalid)
{
    BOOST_CHECK_EQUAL(Color("000000"),   Color(0x00, 0x00, 0x00));
    BOOST_CHECK_EQUAL(Color("00000000"), Color(0x00, 0x00, 0x00, 0x00));

    BOOST_CHECK_EQUAL(Color("FFFFFF"),   Color(0xFF, 0xFF, 0xFF));
    BOOST_CHECK_EQUAL(Color("FFFFFFFF"), Color(0xFF, 0xFF, 0xFF, 0xFF));
    BOOST_CHECK_EQUAL(Color("ffffff"),   Color(0xFF, 0xFF, 0xFF));
    BOOST_CHECK_EQUAL(Color("ffffffff"), Color(0xFF, 0xFF, 0xFF, 0xFF));

    BOOST_CHECK_EQUAL(Color("800000"), Color(0x80, 0x00, 0x00));
    BOOST_CHECK_EQUAL(Color("008000"), Color(0x00, 0x80, 0x00));
    BOOST_CHECK_EQUAL(Color("000080"), Color(0x00, 0x00, 0x80));
    BOOST_CHECK_EQUAL(Color("00000080"), Color(0x00, 0x00, 0x00, 0x80));
}


BOOST_AUTO_TEST_CASE(TestColorToString)
{
    std::string expect;
    Color col;

    col = Color { 0x80, 0x40, 0x20 };
    expect = "804020";
    BOOST_CHECK_EQUAL(col.toString(), expect);
    BOOST_CHECK_EQUAL(col.toStringRGB(), expect);
    BOOST_CHECK_EQUAL(static_cast<std::string>(col), expect);

    col = Color { 0x80, 0x40, 0x20, 0x10 };
    expect = "80402010";
    BOOST_CHECK_EQUAL(col.toString(), expect);
    BOOST_CHECK_EQUAL(col.toStringRGBA(), expect);
    BOOST_CHECK_EQUAL(static_cast<std::string>(col), expect);

}


BOOST_AUTO_TEST_CASE(TestStringColor)
{

    // Invalid strings
    BOOST_CHECK_THROW(Color(""), std::invalid_argument);
    BOOST_CHECK_THROW(Color("Bad"), std::invalid_argument);

    // Right length but invalid strings
    BOOST_CHECK_THROW(Color("1234567"), std::invalid_argument);
    BOOST_CHECK_THROW(Color("123456789"), std::invalid_argument);

    // Not enough values (length should be 7 or 9)
    BOOST_CHECK_THROW(Color("00000"), std::invalid_argument);
    BOOST_CHECK_THROW(Color("0000000"), std::invalid_argument);

    // Should be hex
    BOOST_CHECK_THROW(Color("000g00"), std::invalid_argument);
    BOOST_CHECK_THROW(Color("0000000x"), std::invalid_argument);
}



BOOST_AUTO_TEST_CASE(TestBlend)
{
    Color col { Color::TRANSPARENT };

    col << Color::BLACK;
    BOOST_CHECK_EQUAL(col, Color::BLACK);

    col = Color::TRANSPARENT;
    col << Color::BLACK << Color::WHITE << Color::RED << Color::BLUE;
    BOOST_CHECK_EQUAL(col, Color::BLUE);

    col = Color::BLACK;
    col << Color(0xFF, 0x00, 0x00, 0x80);
    BOOST_CHECK_EQUAL(col, Color(0x80, 0x00, 0x00, 0xFF));

    col = Color::BLACK;
    col << Color(0xFF, 0x00, 0x00, 0xFF) << Color(0x00, 0xFF, 0x00, 0x80);
    BOOST_CHECK_EQUAL(col, Color(0x80, 0x80, 0x00, 0xFF));

    col = Color::BLACK;
    col << Color(0x00, 0xFF, 0x00, 0xFF) << Color(0x00, 0x00, 0xFF, 0x80);
    BOOST_CHECK_EQUAL(col, Color(0x00, 0x80, 0x80, 0xFF));

    col = Color::BLACK;
    col << Color(0x00, 0x00, 0xFF, 0xFF) << Color(0xFF, 0x00, 0x00, 0x80);
    BOOST_CHECK_EQUAL(col, Color(0x80, 0x00, 0x80, 0xFF));

    col = Color::GREEN;
    col << Color(0x00,0x00,0x00,0x80);
    BOOST_CHECK_EQUAL(col, Color(0x00, 0x80, 0x00, 0xFF));

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
        BOOST_CHECK_EQUAL(c, expected);
        BOOST_CHECK_EQUAL(c2, expected);

        b = 0.5f;
        c = source * b;
        c2 = source;
        c2*= b;
        expected = Color { static_cast<uint8_t>(source.red()?0x80:0x00), static_cast<uint8_t>(source.green()?0x80:0x00), static_cast<uint8_t>(source.blue()?0x80:0x00) };
        BOOST_CHECK_EQUAL(c, expected);
        BOOST_CHECK_EQUAL(c2, expected);
        BOOST_CHECK_EQUAL(c, source * b);
        BOOST_CHECK_EQUAL(c2, source * b);

        c = source * 1.0f;
        c2 = source;
        c2*= 1.0f;
        expected = source;
        BOOST_CHECK_EQUAL(c, expected);
        BOOST_CHECK_EQUAL(c2, expected);

        // Check that we round brightness correctly
        b = 254.5f/255.0f;
        c = source * b;
        c2 = source;
        c2*= b;
        expected = source;
        BOOST_CHECK_EQUAL(c, expected);
        BOOST_CHECK_EQUAL(c2, expected);

        b = 0.4999f/255.0f;
        c = source * b;
        c2 = source;
        c2*= b;
        expected = Color::BLACK;
        BOOST_CHECK_EQUAL(c, expected);
        BOOST_CHECK_EQUAL(c2, expected);

    }

}

BOOST_AUTO_TEST_CASE(TestColorCorrection)
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
            BOOST_CHECK_EQUAL(c, expected);
            BOOST_CHECK_EQUAL(c2, expected);
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
        BOOST_CHECK_EQUAL(colors[i], test_colors[i]);
        BOOST_CHECK_EQUAL(colors2[i], test_colors[i]);
        BOOST_CHECK_EQUAL(colors3[i], test_colors[i]);
    }

    // Check brightness
    colors2 *= 0x80;
    for (uint i=0u; i<colors.size(); i++) {
        BOOST_CHECK_EQUAL(colors2[i], test_colors[i] * 0x80);
    }

    // Check correction
    auto cor = Color::Correction::TypicalLEDStrip;
    colors3 *= cor;
    for (uint i=0u; i<colors.size(); i++) {
        BOOST_CHECK_EQUAL(colors3[i], test_colors[i] * cor);
    }


    // Test ColorArray to RawColorArray conversion
    RawColorArray<ARRAY_SIZE> raw_colors { colors };
    for (uint i=0u; i<colors.size(); i++) {
        BOOST_CHECK_EQUAL(raw_colors[i], colors[i].raw());
    }

}


BOOST_AUTO_TEST_CASE(TestSegments)
{
    Robot::LED::ColorArray<4> array { 
        { "seg0", 0, 2 },
        { "seg1", 2, 2 },
        { "seg2", 1, 2 },
    };

    array[0] = Color::RED;
    array[1] = Color::GREEN;
    array[2] = Color::BLUE;
    array[3] = Color::YELLOW;

    BOOST_CHECK_EQUAL(array[0], Color::RED);
    BOOST_CHECK_EQUAL(array[1], Color::GREEN);
    BOOST_CHECK_EQUAL(array[2], Color::BLUE);
    BOOST_CHECK_EQUAL(array[3], Color::YELLOW);

    BOOST_CHECK_EQUAL(array.segments()[0][0], Color::RED);
    BOOST_CHECK_EQUAL(array.segments()[0][1], Color::GREEN);

    BOOST_CHECK_EQUAL(array.segments()[1][0], Color::BLUE);
    BOOST_CHECK_EQUAL(array.segments()[1][1], Color::YELLOW);

    BOOST_CHECK_EQUAL(array.segments()[2][0], Color::GREEN);
    BOOST_CHECK_EQUAL(array.segments()[2][1], Color::BLUE);

    array.fill(Color::BLACK);
    array.segments()[2].fill(Color::WHITE);

    BOOST_CHECK_EQUAL(array[0], Color::BLACK);
    BOOST_CHECK_EQUAL(array[1], Color::WHITE);
    BOOST_CHECK_EQUAL(array[2], Color::WHITE);
    BOOST_CHECK_EQUAL(array[3], Color::BLACK);

}

BOOST_AUTO_TEST_CASE(TestSegmentsFind)
{
    using array_type = Robot::LED::ColorArray<4>;

    array_type array { 
        { "seg0", 0, 2 },
        { "seg1", 2, 2 },
        { "seg2", 1, 2 },
    };

    BOOST_CHECK_EQUAL(array.segments()[0], array_type::Segment("seg0", 0, 2));
    BOOST_CHECK_EQUAL(array.segment("seg0"), array_type::Segment("seg0", 0, 2));
    BOOST_CHECK_EQUAL(array.segments().find("seg0"), array_type::Segment("seg0", 0, 2));

    BOOST_CHECK_EQUAL(array.segments()[1], array_type::Segment("seg1", 2, 2));
    BOOST_CHECK_EQUAL(array.segment("seg1"), array_type::Segment("seg1", 2, 2));
    BOOST_CHECK_EQUAL(array.segments().find("seg1"), array_type::Segment("seg1", 2, 2));

    BOOST_CHECK_EQUAL(array.segments()[2], array_type::Segment("seg2", 1, 2));
    BOOST_CHECK_EQUAL(array.segment("seg2"), array_type::Segment("seg2", 1, 2));
    BOOST_CHECK_EQUAL(array.segments().find("seg2"), array_type::Segment("seg2", 1, 2));


    BOOST_CHECK_THROW(array.segments().find("none"), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(TestSegmentsFindConst)
{
    using array_type = Robot::LED::ColorArray<4>;

    const array_type array { 
        { "seg0", 0, 2 },
        { "seg1", 2, 2 },
        { "seg2", 1, 2 },
    };

    BOOST_CHECK_EQUAL(array.segments()[0], array_type::Segment("seg0", 0, 2));
    BOOST_CHECK_EQUAL(array.segment("seg0"), array_type::Segment("seg0", 0, 2));
    BOOST_CHECK_EQUAL(array.segments().find("seg0"), array_type::Segment("seg0", 0, 2));

    BOOST_CHECK_EQUAL(array.segments()[1], array_type::Segment("seg1", 2, 2));
    BOOST_CHECK_EQUAL(array.segment("seg1"), array_type::Segment("seg1", 2, 2));
    BOOST_CHECK_EQUAL(array.segments().find("seg1"), array_type::Segment("seg1", 2, 2));

    BOOST_CHECK_EQUAL(array.segments()[2], array_type::Segment("seg2", 1, 2));
    BOOST_CHECK_EQUAL(array.segment("seg2"), array_type::Segment("seg2", 1, 2));
    BOOST_CHECK_EQUAL(array.segments().find("seg2"), array_type::Segment("seg2", 1, 2));


    BOOST_CHECK_THROW(array.segments().find("none"), std::out_of_range);
}



BOOST_AUTO_TEST_SUITE_END()

