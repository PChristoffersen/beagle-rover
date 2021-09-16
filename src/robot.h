#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include "component.h"

class Robot {
    public:
        Robot();
        virtual ~Robot();

        void init();
        void cleanup();

        boost::shared_ptr<class MotorControl> motorControl() { return m_motor_control; }
        boost::shared_ptr<class LEDControl> ledControl() { return m_led_control; }
        boost::shared_ptr<class Telemetry> telemetry() { return m_telemetry; }

    private:
        bool m_initialized;
        boost::shared_ptr<boost::asio::io_context> m_io;
        boost::shared_ptr<boost::thread> m_thread;

        boost::shared_ptr<class FBus> m_fbus;
        boost::shared_ptr<class MotorControl> m_motor_control;
        boost::shared_ptr<class LEDControl> m_led_control;
        boost::shared_ptr<class Telemetry> m_telemetry;
        boost::shared_ptr<class PRUDebug> m_pru_debug;
};

#endif