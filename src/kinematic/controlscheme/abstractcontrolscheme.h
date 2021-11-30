#ifndef _ABSTRACTCONTROLSCHEME_H_
#define _ABSTRACTCONTROLSCHEME_H_

#include <memory>
#include <mutex>
#include "../controlscheme.h"
#include "../kinematic.h"
#include "../../telemetry/telemetry.h"
#include "../../motor/motorcontrol.h"
#include "../../rc/rcreceiver.h"

namespace Robot::Kinematic {

    template<typename T>
    class AbstractControlScheme : public ControlScheme, public std::enable_shared_from_this<T> {
        public:
            virtual ~AbstractControlScheme() { }

            virtual void updateSteeringMode(SteeringMode mode) {}
            virtual void updateDriveMode(DriveMode mode) {}
            virtual void updateOrientation(Orientation orientation) {}

            virtual void steer(double steering, double throttle, double aux_x, double aux_y) {}

        protected:
            AbstractControlScheme(std::shared_ptr<Kinematic> kinematic) :
                m_initialized { false },
                m_kinematic { kinematic },
                m_context { kinematic->context() },
                m_motor_control { kinematic->motorControl() }
            {
            }

            bool m_initialized;
            std::mutex m_mutex;
            std::shared_ptr<Robot::Context> m_context;
            std::weak_ptr<Kinematic> m_kinematic;
            std::shared_ptr<Robot::Motor::Control> m_motor_control;
    };

};

#endif
