#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <memory>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "robotcontext.h"
#include "rc/rcreceiver.h"
#include "motor/motorcontrol.h"
#include "led/ledcontrol.h"
#include "telemetry/telemetry.h"
#include "kinematic/kinematic.h"
#include "debug/prudebug.h"

namespace Robot {

    class Robot {
        public:
            Robot();
            Robot(const Robot&) = delete; // No copy constructor
            Robot(Robot&&) = delete; // No move constructor

            virtual ~Robot();


            void init();
            void cleanup();

            void setArmed(bool enable);
            bool getArmed() const { return m_armed; }

            const std::shared_ptr<Context> &context() const { return m_context; }
            const std::shared_ptr<RC::Receiver> &rcReceiver() const { return m_rc_receiver; }
            const std::shared_ptr<Motor::Control> &motorControl() const { return m_motor_control; }
            const std::shared_ptr<LED::Control> &ledControl() const { return m_led_control; }
            const std::shared_ptr<Telemetry::Telemetry> &telemetry() const { return m_telemetry; }
            const std::shared_ptr<Kinematic::Kinematic> &kinematic() const { return m_kinematic; }

        private:
            static class Robot *m_instance;
            bool m_initialized;
            bool m_armed;
            std::shared_ptr<Context> m_context;

            std::shared_ptr<RC::Receiver> m_rc_receiver;
            std::shared_ptr<Motor::Control> m_motor_control;
            std::shared_ptr<LED::Control> m_led_control;
            std::shared_ptr<Telemetry::Telemetry> m_telemetry;
            std::shared_ptr<Kinematic::Kinematic> m_kinematic;
            std::shared_ptr<PRU::Debug> m_pru_debug;

    };

};

#endif