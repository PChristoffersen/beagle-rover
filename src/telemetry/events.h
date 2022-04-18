#ifndef _ROBOT_TELEMETRY_EVENTS_H_
#define _ROBOT_TELEMETRY_EVENTS_H_

#include <cstdint>
#include <vector>
#include <array>
#include <string_view>
#include "types.h"
#include <motor/types.h>

namespace Robot::Telemetry {

    class Event {
        public:
            std::string_view name;
            Event(const std::string_view &name) : name { name } {}
            virtual ~Event() = default;
            virtual void update(ValueMap &map) const = 0;
    };

    class EventMotors : public Event {
        public:
            using duty_type = float;
            using rpm_type = float;
            using duty_list = std::array<duty_type, Motor::MOTOR_COUNT>;
            using rpm_list = std::array<rpm_type, Motor::MOTOR_COUNT>;

            duty_list duty;
            rpm_list rpm;
            rpm_list rpm_target;

            EventMotors(const std::string_view &name) : 
                Event { name }
            {
                duty.fill(0.0f);
                rpm.fill(0.0f);
                rpm_target.fill(0.0f);
            }
            EventMotors() : EventMotors { "" } {}
            virtual void update(ValueMap &map) const;
    };

    class EventBattery : public Event {
        public:
            EventBattery(const std::string_view &name) : 
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
            EventTemperature(const std::string_view &name) : Event { name }, temperature { 0.0f } {}
            float temperature;
            virtual void update(ValueMap &map) const;
    };

    class EventOdometer : public Event {
        public:
            EventOdometer(const std::string_view &name) : Event { name }, value { 0 } {}
            EventOdometer() : EventOdometer { "" } {}
            std::int32_t value;
            virtual void update(ValueMap &map) const;
    };

    class EventIMU : public Event {
        public:
            EventIMU(const std::string_view &name, float pitch, float roll, float yaw) : 
                Event { name },
                pitch { pitch },
                roll { roll },
                yaw { yaw }
            {
            }
            EventIMU() : 
                EventIMU { "", 0.0f, 0.0f, 0.0f } 
            {
            }
            float pitch;
            float roll;
            float yaw;
            virtual void update(ValueMap &map) const;
    };

}


#endif
