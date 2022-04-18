#ifndef _ROBOT_LED_TYPES_H_
#define _ROBOT_LED_TYPES_H_

#include <initializer_list>

#include <robotconfig.h>
#include "color.h"
#include "colorarray.h"


namespace Robot::LED {

    class Control;
    class Color;
    class ColorLayer;

    /**
     * @brief Number of LED pixels in the robot pixel string
     * 
     */
    constexpr uint PIXEL_COUNT { ROBOT_NEOPIXEL_COUNT };


    enum class AnimationMode {
        NONE,
        HEADLIGHTS,
        CONSTRUCTION,
        POLICE,
        AMBULANCE,
        RUNNING_LIGHT,
        KNIGHT_RIDER,
        RAINBOW,
        RAINBOW_WAVE,
    };

    enum class IndicatorMode {
        NONE,
        LEFT,
        RIGHT,
        HAZARD
    };

    using color_array_type = ColorArray<PIXEL_COUNT>;

    /**
     * @brief Default segment list 
     * 
     * Defined in control.cpp
     */
    const std::initializer_list<color_array_type::Segment> DEFAULT_SEGMENTS {
        { "front", 0, PIXEL_COUNT/2 }, 
        { "back",  PIXEL_COUNT/2, PIXEL_COUNT/2 }, 
    };
    constexpr auto FRONT_SEGMENT { 0u };
    constexpr auto BACK_SEGMENT { 1u };

}

#endif
