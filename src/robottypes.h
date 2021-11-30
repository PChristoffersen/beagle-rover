#ifndef _ROBOTTYPES_H_
#define _ROBOTTYPES_H_

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
    };
    namespace System {
        class WiFi;
        class PRUDebug;
    };
};

#endif
