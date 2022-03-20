#ifndef _ROBOT_CONTEXT_H_
#define _ROBOT_CONTEXT_H_

#include <mutex>
#include <memory>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <robotconfig.h>
#include <common/withmutex.h>
#include <common/withnotify.h>
#include <common/properties.h>
#include <hardware/types.h>

namespace Robot {

    class Context : public std::enable_shared_from_this<Context>, public WithMutexRecursive {
        public:
            using power_signal_type = boost::signals2::signal<void(bool)>;

            Context();
            Context(const Context&) = delete; // No copy constructor
            Context(Context&&) = delete; // No move constructor

            virtual ~Context();

            void init();
            void cleanup();

            void start();
            void stop();

            void motorPower(bool enable);
            void servoPower(bool enable);
            void ledPower(bool enable);
            void rcPower(bool enable);
            bool motorPower() const { return m_motor_power_enabled; }
            bool servoPower() const { return m_servo_power_enabled; }
            bool ledPower() const { return m_led_power_enabled; }
            bool rcPower() const { return m_rc_power_enabled; }

            uint heartbeat() const { return m_heartbeat; }

            boost::asio::io_context &io() { return m_io; }


            const Properties &properties() const { return m_properties; }
            Properties &properties() { return m_properties; }
            const PropertyMap &properties(const std::string &group) const { return m_properties.at(group); }
            PropertyMap &properties(const std::string &group) { return m_properties.at(group); }

            void registerProperties(const std::string &group, const PropertyMap &values);

            power_signal_type sig_motor_power;
            power_signal_type sig_servo_power;
            power_signal_type sig_led_power;
            power_signal_type sig_rc_power;

        private:
            bool m_initialized;
            bool m_started;
            boost::asio::io_context m_io;
            std::vector<std::unique_ptr<std::thread>> m_thread_pool;
            
            boost::asio::steady_timer m_timer;
            uint m_heartbeat;

            void timerSetup();

            Properties m_properties;

            bool m_motor_power_enabled;
            bool m_servo_power_enabled;
            bool m_led_power_enabled;
            bool m_rc_power_enabled;
            std::shared_ptr<::Robot::Hardware::AbstractPower> m_motor_power;
            std::shared_ptr<::Robot::Hardware::AbstractPower> m_servo_power;
            std::shared_ptr<::Robot::Hardware::AbstractPower> m_led_power;
            std::shared_ptr<::Robot::Hardware::AbstractPower> m_rc_power;

            void initPlatform();
            void cleanupPlatform();

            void setPowerEnabled(std::shared_ptr<::Robot::Hardware::AbstractPower> &power, power_signal_type &sig_power, bool enabled, bool &state, const char *name);
    };

}

#endif
