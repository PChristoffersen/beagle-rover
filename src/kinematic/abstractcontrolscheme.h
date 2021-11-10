#ifndef _ABSTRACTCONTROLSCHEME_H_
#define _ABSTRACTCONTROLSCHEME_H_

#include <memory>
#include <mutex>
#include "controlscheme.h"
#include "kinematic.h"

template<typename T>
class AbstractControlScheme : public ControlScheme, public std::enable_shared_from_this<T> {
    public:
        virtual ~AbstractControlScheme() { }

    protected:
        AbstractControlScheme(std::shared_ptr<class Kinematic> kinematic);

        bool m_initialized;
        std::mutex m_mutex;
        std::weak_ptr<class Kinematic> m_kinematic;
        std::shared_ptr<class RobotContext> m_context;
        std::shared_ptr<class MotorControl> m_motor_control;
        std::shared_ptr<class Telemetry> m_telemetry;
        std::shared_ptr<class RCReceiver> m_rc_receiver;
};


template<typename T>
AbstractControlScheme<T>::AbstractControlScheme(std::shared_ptr<Kinematic> kinematic) :
    m_initialized { false },
    m_kinematic { kinematic },
    m_context { kinematic->context() },
    m_motor_control { kinematic->motorControl() },
    m_telemetry { kinematic->telemetry() },
    m_rc_receiver { kinematic->rc_receiver() }
{

}

#endif
