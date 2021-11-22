#ifndef _TELEMETRYTYPES_H_
#define _TELEMETRYTYPES_H_

#include <cstdint>
#include <vector>
#include <boost/signals2.hpp>

namespace Robot::Telemetry {

    using Signal = boost::signals2::signal<void(const class Event &)>;


    class Event {
        public:
            virtual ~Event() = default;
    };


    class EventBattery : public Event {
        public:
            EventBattery() {}
            std::uint8_t battery_id;
            float voltage;
            std::vector<float> cell_voltage;
    };

};

#endif