#ifndef _ROBOT_TELEMETRY_TYPES_H_
#define _ROBOT_TELEMETRY_TYPES_H_

#include <cstdint>
#include <string>
#include <string_view>
#include <variant>
#include <array>
#include <map>
#include <boost/signals2.hpp>

#include <robotconfig.h>

namespace Robot::Telemetry {

    class Telemetry;

    using Signal = boost::signals2::signal<void(const class Event &)>;

    #if ROBOT_HAVE_IMU
    #if ROBOT_HAVE_ROBOTCONTROL_MPU
    using IMUData = rc_mpu_data_t;
    #else
    using IMUData = char; // Dummy data
    #endif
    using IMUSignal = boost::signals2::signal<void(const IMUData &)>;
    #endif

    using Value = std::variant<std::string, bool, double, float, std::uint32_t, std::int32_t>;
    using ValueMap = std::map<std::string, Value>;

}

#endif