#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

class Robot {
    public:
        Robot();
        ~Robot();

        void init();
        void cleanup();

        void setArmed(bool enable);
        bool getArmed() const { return m_armed; }

        boost::shared_ptr<class RobotContext> context() const { return m_context; }
        boost::shared_ptr<class RCReceiver> rcReceiver() const { return m_rc_receiver; }
        boost::shared_ptr<class MotorControl> motorControl() const { return m_motor_control; }
        boost::shared_ptr<class LEDControl> ledControl() const { return m_led_control; }
        boost::shared_ptr<class Telemetry> telemetry() const { return m_telemetry; }

    private:
        bool m_initialized;
        bool m_armed;
        boost::shared_ptr<class RobotContext> m_context;

        boost::shared_ptr<class RCReceiver> m_rc_receiver;
        boost::shared_ptr<class MotorControl> m_motor_control;
        boost::shared_ptr<class LEDControl> m_led_control;
        boost::shared_ptr<class Telemetry> m_telemetry;
        boost::shared_ptr<class PRUDebug> m_pru_debug;

        void initBeagleBone();
        void cleanupBeagleBone();
        void initPC();
        void cleanupPC();
};

#endif