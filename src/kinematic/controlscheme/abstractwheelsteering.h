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

            virtual void steer(double steering, double throttle, double aux_x, double aux_y) override;


        protected:
            AbstractWheelSteering(std::shared_ptr<Kinematic> kinematic, double wheel_base_factor);

            void resetMotors();
            virtual void setMotors(double left, double right) = 0;

        private:
            const double m_wheel_base_factor;
    };

};

#endif
