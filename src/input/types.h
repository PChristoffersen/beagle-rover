#ifndef _ROBOT_INPUT_TYPES_H_
#define _ROBOT_INPUT_TYPES_H_

#include <iostream>
#include <boost/signals2.hpp>

namespace Robot::Input {

    using SignalSteer = boost::signals2::signal<void(double steer, double throttle, double aux_x, double aux_y)>;

    struct Signals {
        SignalSteer steer;
    };

    static constexpr auto STEER_MIN { -1.0 };
    static constexpr auto STEER_MAX {  1.0 };
    static constexpr auto THROTTLE_MIN { -1.0 };
    static constexpr auto THROTTLE_MAX {  1.0 };

};

#endif
