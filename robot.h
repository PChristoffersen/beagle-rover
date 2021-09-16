#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "component.h"

class Robot : public Component {
    public:
        Robot(boost::asio::io_context &io);
        virtual ~Robot();

        void init() override;
        void cleanup() override;

    private:
        boost::shared_ptr<class PRUDebug> m_pru_debug;
        boost::shared_ptr<class MotorControl> m_motor_control;
        boost::shared_ptr<class LEDControl> m_led_control;
        boost::shared_ptr<class Telemetry> m_telemetry;
};

#endif