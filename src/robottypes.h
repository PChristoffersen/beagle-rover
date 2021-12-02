#ifndef _ROBOT_TYPES_H_
#define _ROBOT_TYPES_H_

#include <iostream>
#include <cinttypes>
#include <cmath>
#include <algorithm>
#include <boost/format.hpp>

namespace Robot {
    class Context;

    namespace RC {
        class Receiver;
    };
    namespace Input {
        class Value;
        class Control;
    };
    namespace Kinematic {
        class Kinematic;
    };
    namespace Telemetry {
        class Telemetry;
    };
    namespace Motor {
        class Control;
    };
    namespace LED {
        class Control;
        class ColorLayer;
        class Color;
    };
    namespace System {
        class WiFi;
        class PRUDebug;
    };
};

#endif
