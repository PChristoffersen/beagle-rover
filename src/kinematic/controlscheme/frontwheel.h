#ifndef _ROBOT_KINEMATIC_CONTROLSCHEME_FRONTWHEEL_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEME_FRONTWHEEL_H_

#include <memory>

#include "abstractwheelsteering.h"

namespace Robot::Kinematic {

    class ControlSchemeFrontWheel : public AbstractWheelSteering, public std::enable_shared_from_this<ControlSchemeFrontWheel> {
        public: 
            explicit ControlSchemeFrontWheel(std::shared_ptr<Kinematic> kinematic);
            ControlSchemeFrontWheel(const ControlSchemeFrontWheel&) = delete; // No copy constructor
            ControlSchemeFrontWheel(ControlSchemeFrontWheel&&) = delete; // No move constructor
            virtual ~ControlSchemeFrontWheel();

        protected:
            virtual void setMotors(float left, float right, float skew) override;
            virtual void setMotorDuty(float steering, float throttle, float outer_dist, float inner_dist, float inner_angle) override;
    };

}

#endif
