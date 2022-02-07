#ifndef _ROBOT_ROBOT_H_
#define _ROBOT_ROBOT_H_

#include <memory>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include <robotconfig.h>
#include <robottypes.h>

namespace Robot {


    class Robot : public std::enable_shared_from_this<Robot> {
        public:
            Robot();
            Robot(const Robot&) = delete; // No copy constructor
            Robot(Robot&&) = delete; // No move constructor

            virtual ~Robot();

            void init();
            void cleanup();

            const std::shared_ptr<class Context> &context() const { return m_context; }
            #if ROBOT_HAVE_RC
            const std::shared_ptr<class RC::Receiver> &rcReceiver() const { return m_rc_receiver; }
            #endif
            const std::shared_ptr<class Motor::Control> &motorControl() const { return m_motor_control; }
            const std::shared_ptr<class LED::Control> &ledControl() const { return m_led_control; }
            const std::shared_ptr<class Telemetry::Telemetry> &telemetry() const { return m_telemetry; }
            const std::shared_ptr<class Kinematic::Kinematic> &kinematic() const { return m_kinematic; }
            const std::shared_ptr<class Input::Control> &input() const { return m_input; }
            #if ROBOT_HAVE_WIFI
            const std::shared_ptr<class System::WiFi> &wifi() const { return m_wifi; }
            #endif

            uint heartbeat() const { return m_heartbeat; }

        private:
            class LogConfig {
                public:
                    LogConfig();
            };

            static class Robot *m_instance;
            bool m_initialized;
            LogConfig m_log_config;
            std::shared_ptr<Context> m_context;
            boost::asio::high_resolution_timer m_timer;

            uint m_heartbeat;

            #if ROBOT_HAVE_RC
            std::shared_ptr<class RC::Receiver> m_rc_receiver;
            #endif
            std::shared_ptr<class Motor::Control> m_motor_control;
            std::shared_ptr<class LED::Control> m_led_control;
            std::shared_ptr<class Telemetry::Telemetry> m_telemetry;
            std::shared_ptr<class Kinematic::Kinematic> m_kinematic;
            std::shared_ptr<class Input::Control> m_input;
            #if ROBOT_HAVE_WIFI
            std::shared_ptr<class System::WiFi> m_wifi;
            #endif
            #if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE
            std::shared_ptr<class System::PRUDebug> m_pru_debug;
            #endif

            void timerSetup();
            void timer();
    };

};

#endif