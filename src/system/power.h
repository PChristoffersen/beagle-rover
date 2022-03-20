#ifndef _ROBOT_SYSTEM_POWER_H_
#define _ROBOT_SYSTEM_POWER_H_

#include <memory>
#include <mutex>

#include <robotconfig.h>
#include <robottypes.h>
#include <common/withmutex.h>
#include <common/withnotify.h>
#include <telemetry/types.h>
#include <telemetry/events.h>

namespace Robot::System {

    class PowerSource : public WithMutexStd {
        public:
            enum class Type {
                UNKNOWN,
                BATTERY,
            };
            PowerSource(Type type, const std::string &name);
            PowerSource(const PowerSource&) = delete; // No copy constructor
            PowerSource(PowerSource&&) = delete; // No move constructor
            virtual ~PowerSource() {}

            Type type() const { return m_type; }
            std::string name() const { return m_name; }

        private:
            Type m_type;
            std::string m_name;
    };

    class PowerSourceUnknown : public PowerSource {
        public:
            PowerSourceUnknown(const std::string &name);
    };

    class PowerSourceBattery : public PowerSource {
        public:
            PowerSourceBattery(const std::string &name);

            bool charging() const { return m_charging; }
            bool onBattery() const { return m_on_battery; }
            float jackVoltage() const { return m_jack_voltage; }
            float percent() const { return m_percent; }
            float voltage() const { return m_voltage; }

            void test(float v) { m_voltage = v; }

            void set(const ::Robot::Telemetry::EventBattery &event);
        private:
            bool m_charging; // Is the battery charging
            bool m_on_battery; // Is the robot running on battery
            float m_jack_voltage; // Voltage of the power supply
            float m_percent; // Pattery charge percent
            float m_voltage; // Total battery voltage
    };

    class Power : public std::enable_shared_from_this<Power>, public WithMutexStd, public WithNotifyInt {
        public:
            explicit Power(const std::shared_ptr<::Robot::Context> &context);
            Power(const Power&) = delete; // No copy constructor
            Power(Power&&) = delete; // No move constructor
            virtual ~Power();

            void init(const std::shared_ptr<::Robot::Telemetry::Telemetry> &telemetry);
            void cleanup();
        
            const std::unique_ptr<PowerSource> &system() const { return m_system_power; }

        private:
            bool m_initialized;

            boost::signals2::connection m_telemetry_connection;

            std::unique_ptr<PowerSource> m_system_power;

            void onTelemetryEvent(const ::Robot::Telemetry::Event &event);

            friend std::ostream &operator<<(std::ostream &os, const Power &self)
            {
                return os << "Robot::System::Power";
            }
    };

}


#endif
