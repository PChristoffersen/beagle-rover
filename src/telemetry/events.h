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

    class EventMotor : public Event {
        public:
            EventMotor(const std::string &name, uint index) : 
                Event { name }, 
                index { index },
                enabled { false },
                duty { 0.0f },
                rpm { 0.0f },
                rpm_target { -1.0f }
            {}
            uint index;
            bool enabled;
            float duty;
            float rpm;
            float rpm_target;
            virtual void update(ValueMap &map) const;
    };

    class EventServo : public Event {
        public:
            EventServo(const std::string &name, uint index) : 
                Event { name }, 
                index { index },
                enabled { false },
                angle { 0.0f }
            {}
            uint index;
            bool enabled;
            float angle;
            virtual void update(ValueMap &map) const;
    };

    class EventBattery : public Event {
        public:
            EventBattery(const std::string &name) : 
                Event { name },
                battery_id { 0x00 },
                charging { false },
                on_battery { true },
                jack_voltage { 0.0f },
                percent { 0.0f },
                voltage { 0.0f }
            { 
            }
            std::uint8_t battery_id;
            bool charging; // Is the battery charging
            bool on_battery; // Is the robot running on battery
            float jack_voltage; // Voltage of the power supply
            float percent; // Pattery charge percent
            float voltage; // Total battery voltage
            std::vector<float> cell_voltage;
            virtual void update(ValueMap &map) const;
    };

    class EventTemperature : public Event {
        public:
            EventTemperature(const std::string &name) : Event { name } {}
            float temperature;
            virtual void update(ValueMap &map) const;
    };

    class EventIMU : public Event {
        public:
            EventIMU(const std::string &name, float pitch, float roll, float yaw) : 
                Event { name },
                pitch { pitch },
                roll { roll },
                yaw { yaw }
            {
            }
            EventIMU(const std::string &name) : 
                EventIMU { name, 0.0f, 0.0f, 0.0f } 
            {
            }
            float pitch;
            float roll;
            float yaw;
            virtual void update(ValueMap &map) const;
    };

}


#endif
