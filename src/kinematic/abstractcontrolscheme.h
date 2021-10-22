#ifndef _CONTROLSCHEME_H_
#define _CONTROLSCHEME_H_

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class AbstractControlScheme : public boost::enable_shared_from_this<AbstractControlScheme> {
    public:
        virtual ~AbstractControlScheme();

        virtual void init() = 0;
        virtual void cleanup() = 0;

    protected:
        AbstractControlScheme(boost::shared_ptr<class Kinematic> kinematic);

        boost::weak_ptr<class Kinematic> m_kinematic;
        boost::shared_ptr<class RobotContext> m_context;
        boost::shared_ptr<class MotorControl> m_motor_control;
        boost::shared_ptr<class Telemetry> m_telemetry;
};

#endif
