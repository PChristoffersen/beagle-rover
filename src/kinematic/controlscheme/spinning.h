#ifndef _ROBOT_KINEMATIC_CONTROLSCHEMESPINNING_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEMESPINNING_H_

#include <memory>

#include "abstractcontrolscheme.h"

namespace Robot::Kinematic {

    class ControlSchemeSpinning : public AbstractControlScheme, public std::enable_shared_from_this<ControlSchemeSpinning> {
        public: 
            explicit ControlSchemeSpinning(std::shared_ptr<Kinematic> kinematic);
            ControlSchemeSpinning(const ControlSchemeSpinning&) = delete; // No copy constructor
            ControlSchemeSpinning(ControlSchemeSpinning&&) = delete; // No move constructor
            virtual ~ControlSchemeSpinning();

            virtual void init() override;
            virtual void cleanup() override;

            virtual void steer(float steering, float throttle, float aux_x, float aux_y) override;

        private:
    };

};

#endif
