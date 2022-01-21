#ifndef _ROBOT_INPUT_TYPES_H_
#define _ROBOT_INPUT_TYPES_H_

#include <iostream>
#include <boost/signals2.hpp>

#include <common/value.h>

namespace Robot::Input {

    using SignalSteer = boost::signals2::signal<void(float steer, float throttle, float aux_x, float aux_y)>;

    struct Signals {
        SignalSteer steer;
    };

    static constexpr auto STEER_MIN { -1.0f };
    static constexpr auto STEER_MAX {  1.0f };
    static constexpr auto THROTTLE_MIN { -1.0f };
    static constexpr auto THROTTLE_MAX {  1.0f };

    using Value = ::Robot::Value;
};

#endif
