#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <memory>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "robottypes.h"

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
            const std::shared_ptr<class RC::Receiver> &rcReceiver() const { return m_rc_receiver; }
            const std::shared_ptr<class Motor::Control> &motorControl() const { return m_motor_control; }
            const std::shared_ptr<class LED::Control> &ledControl() const { return m_led_control; }
            const std::shared_ptr<class Telemetry::Telemetry> &telemetry() const { return m_telemetry; }
            const std::shared_ptr<class Kinematic::Kinematic> &kinematic() const { return m_kinematic; }
            const std::shared_ptr<class Input::Control> &input() const { return m_input; }
            const std::shared_ptr<class System::WiFi> &wifi() const { return m_wifi; }

            uint heartbeat() const { return m_heartbeat; }

        private:
            static class Robot *m_instance;
            bool m_initialized;
            std::shared_ptr<Context> m_context;
            boost::asio::high_resolution_timer m_timer;

            uint m_heartbeat;

            std::shared_ptr<class RC::Receiver> m_rc_receiver;
            std::shared_ptr<class Motor::Control> m_motor_control;
            std::shared_ptr<class LED::Control> m_led_control;
            std::shared_ptr<class Telemetry::Telemetry> m_telemetry;
            std::shared_ptr<class Kinematic::Kinematic> m_kinematic;
            std::shared_ptr<class Input::Control> m_input;
            std::shared_ptr<class System::PRUDebug> m_pru_debug;
            std::shared_ptr<class System::WiFi> m_wifi;

            void timerSetup();
            void timer();
    };

};

#endif