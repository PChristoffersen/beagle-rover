#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <boost/asio.hpp>

#include "component.h"
#include "motorcontrol.h"
#include "battery.h"

class Robot : public Component {
    public:
        Robot(boost::asio::io_context &io);
        virtual ~Robot();

        void init() override;
        void cleanup() override;

        MotorControl &motor_control() {
            return m_motor_control;
        }
    private:
        MotorControl m_motor_control;
        Battery m_battery;
};

#endif