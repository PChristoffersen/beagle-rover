#ifndef _ROBOT_TELEMETRY_TYPES_H_
#define _ROBOT_TELEMETRY_TYPES_H_

#include <cstdint>
#include <string>
#include <variant>
#include <map>
#include <boost/signals2.hpp>
#include <robotcontrol.h>

namespace Robot::Telemetry {

    class Telemetry;

    using Signal = boost::signals2::signal<void(const class Event &)>;

    using MPUData = rc_mpu_data_t;
    using MPUSignal = boost::signals2::signal<void(const MPUData &)>;

    using Value = std::variant<std::string, double, std::uint32_t>;
    using ValueMap = std::map<std::string, Value>;

};

#endif