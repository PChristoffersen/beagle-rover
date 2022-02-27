#ifndef _ROBOT_KINEMATIC_ABSTRACTCONTROLSCHEME_H_
#define _ROBOT_KINEMATIC_ABSTRACTCONTROLSCHEME_H_

#include <memory>
#include <mutex>

#include <common/withmutex.h>
#include <telemetry/telemetry.h>
#include <motor/control.h>
#include <motor/motor.h>
#include <motor/servo.h>
#include <rc/receiver.h>
#include "../controlscheme.h"
#include "../kinematic.h"

namespace Robot::Kinematic {

    class AbstractControlScheme : public ControlScheme, public WithMutex<std::recursive_mutex> {
        public:
            virtual ~AbstractControlScheme() { }

            virtual void updateOrientation(Orientation orientation) override;

            virtual void steer(float steering, float throttle, float aux_x, float aux_y) override {}

        protected:
            AbstractControlScheme(std::shared_ptr<Kinematic> kinematic);

            bool m_initialized;
            Orientation m_orientation;
            bool m_orientation_reverse;
            MotorMap m_motor_map;
            std::shared_ptr<Robot::Context> m_context;
            std::shared_ptr<Robot::Motor::Control> m_motor_control;

            float m_last_steering;
            float m_last_throttle;
            float m_last_aux_x;
            float m_last_aux_y;

            void setLastSteering(float steering, float throttle, float aux_x, float aux_y) 
            {
                m_last_steering = steering;
                m_last_throttle = throttle;
                m_last_aux_x = aux_x;
                m_last_aux_y = aux_y;
            }

            inline auto &motor(MotorPosition position) const 
            {
                return m_motor_control->getMotors()[m_motor_map[position].index];
            }
            inline void motorServo(MotorPosition position, Value value) {
                auto &entry = m_motor_map[position];
                m_motor_control->getMotors()[entry.index]->servo()->setValue(entry.invert ? -value : value);
            }
            inline void motorDuty(MotorPosition position, float value) {
                auto &entry = m_motor_map[position];
                m_motor_control->getMotors()[entry.index]->setDuty(entry.invert_duty ? -value : value);
            }
            inline void motorSet(MotorPosition position, Value servo, float throttle) {
                auto &entry = m_motor_map[position];
                auto &motor = m_motor_control->getMotors()[entry.index];
                motor->servo()->setValue(entry.invert ? -servo : servo);
                motor->setDuty(entry.invert_duty ? -throttle : throttle);
            }

            virtual void orientationUpdated(Orientation orientation) {
                steer(m_last_steering, m_last_throttle, m_last_aux_x, m_last_aux_y);
            }
    };

}

#endif
