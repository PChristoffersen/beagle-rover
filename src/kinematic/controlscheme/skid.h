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

            virtual void updateOrientation(Orientation orientation) override;

            virtual void steer(double steering, double throttle, double aux_x, double aux_y) override;

        private:
            void resetMotors();
    };

};

#endif
