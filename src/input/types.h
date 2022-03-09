#ifndef _ROBOT_INPUT_TYPES_H_
#define _ROBOT_INPUT_TYPES_H_

#include <iostream>
#include <boost/signals2.hpp>

#include <common/value.h>
#include <kinematic/types.h>
#include <led/types.h>

namespace Robot::Input {


    struct Signals {
        using steer_signal_type = boost::signals2::signal<void(float direction, float throttle, float aux_x, float aux_y)>;
        using drive_mode_signal_type = boost::signals2::signal<void(::Robot::Kinematic::DriveMode drive_mode)>;
        using orientation_signal_type = boost::signals2::signal<void(::Robot::Kinematic::Orientation orientation)>;
        using animation_mode_signal_type = boost::signals2::signal<void(::Robot::LED::AnimationMode animation_mode)>;
        using indicator_mode_signal_type = boost::signals2::signal<void(::Robot::LED::IndicatorMode indicator_mode)>;
        using brightness_signal_type = boost::signals2::signal<void(float brightness)>;
        
        steer_signal_type steer;
        // Kinematic
        drive_mode_signal_type drive_mode;
        orientation_signal_type orientation;
        // LED
        animation_mode_signal_type animation_mode;
        indicator_mode_signal_type indicator_mode;
        brightness_signal_type brightness;
    };

    static constexpr auto STEER_MIN { -1.0f };
    static constexpr auto STEER_MAX {  1.0f };
    static constexpr auto THROTTLE_MIN { -1.0f };
    static constexpr auto THROTTLE_MAX {  1.0f };

    using Value = ::Robot::Value;
}

#endif
