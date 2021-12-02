#ifndef _ROBOT_KINEMATIC_CONTROLSCHEME_REARWHEEL_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEME_REARWHEEL_H_

#include <memory>

#include "abstractwheelsteering.h"

namespace Robot::Kinematic {

    class ControlSchemeRearWheel : public AbstractWheelSteering, public std::enable_shared_from_this<ControlSchemeRearWheel> {
        public: 
            explicit ControlSchemeRearWheel(std::shared_ptr<Kinematic> kinematic);
            ControlSchemeRearWheel(const ControlSchemeRearWheel&) = delete; // No copy constructor
            ControlSchemeRearWheel(ControlSchemeRearWheel&&) = delete; // No move constructor
            virtual ~ControlSchemeRearWheel();

        protected:
            virtual void setMotors(Value left, Value right) override;
    };

};

#endif
