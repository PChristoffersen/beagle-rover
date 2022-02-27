#ifndef _ROBOT_KINEMATIC_CONTROLSCHEME_ALLWHEEL_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEME_ALLWHEEL_H_

#include <memory>

#include "abstractwheelsteering.h"

namespace Robot::Kinematic {

    class ControlSchemeAllWheel : public AbstractWheelSteering, public std::enable_shared_from_this<ControlSchemeAllWheel> {
        public: 
            explicit ControlSchemeAllWheel(std::shared_ptr<Kinematic> kinematic);
            ControlSchemeAllWheel(const ControlSchemeAllWheel&) = delete; // No copy constructor
            ControlSchemeAllWheel(ControlSchemeAllWheel&&) = delete; // No move constructor
            virtual ~ControlSchemeAllWheel();

        protected:
            virtual void setMotors(float left, float right) override;
            virtual void setMotorDuty(float steering, float throttle, float outer_dist, float inner_dist, float inner_angle) override;
    };

}

#endif
