#ifndef _ROBOT_KINEMATIC_CONTROLSCHEME_ABSTRACTWHEELSTEERING_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEME_ABSTRACTWHEELSTEERING_H_

#include <mutex>

#include <motor/motor.h>
#include <motor/servo.h>
#include <motor/control.h>

#include "abstractcontrolscheme.h"
#include "../types.h"

namespace Robot::Kinematic {

    class AbstractWheelSteering : public AbstractControlScheme {
        public: 
            virtual void init() override;
            virtual void cleanup() override;

            virtual void updateOrientation(Orientation orientation) override;

            virtual void steer(float steering, float throttle, float aux_x, float aux_y) override;


        protected:
            AbstractWheelSteering(std::shared_ptr<Kinematic> kinematic, float wheel_base_factor);

            void resetMotors();
            virtual void setMotors(float left, float right) = 0;

        private:
            const float m_wheel_base_factor;
    };

}

#endif
