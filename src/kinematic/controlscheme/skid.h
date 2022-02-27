#ifndef _ROBOT_KINEMATIC_CONTROLSCHEME_SKID_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEME_SKID_H_

#include <memory>

#include "abstractcontrolscheme.h"

namespace Robot::Kinematic {

    class ControlSchemeSkid : public AbstractControlScheme, public std::enable_shared_from_this<ControlSchemeSkid> {
        public: 
            explicit ControlSchemeSkid(std::shared_ptr<Kinematic> kinematic);
            ControlSchemeSkid(const ControlSchemeSkid&) = delete; // No copy constructor
            ControlSchemeSkid(ControlSchemeSkid&&) = delete; // No move constructor
            virtual ~ControlSchemeSkid();

            virtual void init() override;
            virtual void cleanup() override;

            virtual void steer(float steering, float throttle, float aux_x, float aux_y) override;

        private:
            void resetMotors();
    };

}

#endif
