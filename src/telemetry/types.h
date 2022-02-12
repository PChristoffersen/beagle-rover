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

    #if ROBOT_HAVE_MPU
    #if ROBOT_HAVE_ROBOTCONTROL_MPU
    using MPUData = rc_mpu_data_t;
    #else
    using MPUData = char; // Dummy data
    #endif
    using MPUSignal = boost::signals2::signal<void(const MPUData &)>;
    #endif

    using Value = std::variant<std::string, bool, double, float, std::uint32_t>;
    using ValueMap = std::map<std::string, Value>;

}

#endif