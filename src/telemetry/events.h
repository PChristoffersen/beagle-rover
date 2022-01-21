#ifndef _ROBOT_TELEMETRY_EVENTS_H_
#define _ROBOT_TELEMETRY_EVENTS_H_

#include <cstdint>
#include <vector>
#include <string>
#include "types.h"

namespace Robot::Telemetry {

    class Event {
        public:
            const std::string name;
            Event(const std::string &name) : name { name } {}
            virtual ~Event() = default;
            virtual void update(ValueMap &map) const = 0;
    };


    class EventBattery : public Event {
        public:
            EventBattery(const std::string &name) : Event { name } {}
            std::uint8_t battery_id;
            float voltage;
            std::vector<float> cell_voltage;
            virtual void update(ValueMap &map) const;
    };

    class EventTemperature : public Event {
        public:
            EventTemperature(const std::string &name) : Event { name } {}
            float temperature;
            virtual void update(ValueMap &map) const;
    };

    class EventMPU : public Event {
        public:
            EventMPU(const std::string &name) : Event { name } {}
            float pitch;
            float roll;
            float yaw;
            float temp;
            virtual void update(ValueMap &map) const;
    };

};


#endif
