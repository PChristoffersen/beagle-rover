#ifndef _ROBOT_TELEMETRY_TYPES_H_
#define _ROBOT_TELEMETRY_TYPES_H_

#include <cstdint>
#include <string>
#include <variant>
#include <map>
#include <boost/signals2.hpp>

#include <robotconfig.h>

namespace Robot::Telemetry {

    class Telemetry;

    using Signal = boost::signals2::signal<void(const class Event &)>;

    #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
    using MPUData = rc_mpu_data_t;
    #else
    using MPUData = uint32_t;
    #endif
    using MPUSignal = boost::signals2::signal<void(const MPUData &)>;

    using Value = std::variant<std::string, double, float, std::uint32_t>;
    using ValueMap = std::map<std::string, Value>;

};

#endif