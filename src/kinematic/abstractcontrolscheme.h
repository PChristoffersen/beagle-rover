#ifndef _CONTROLSCHEME_H_
#define _CONTROLSCHEME_H_

#include <memory>

class AbstractControlScheme {
    public:
        virtual ~AbstractControlScheme();

        virtual void init() = 0;
        virtual void cleanup() = 0;

    protected:
        AbstractControlScheme(std::shared_ptr<class Kinematic> kinematic);

        std::weak_ptr<class Kinematic> m_kinematic;
        std::shared_ptr<class RobotContext> m_context;
        std::shared_ptr<class MotorControl> m_motor_control;
        std::shared_ptr<class Telemetry> m_telemetry;
};

#endif
