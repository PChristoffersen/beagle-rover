#ifndef _ROBOT_KINEMATIC_CONTROLSCHEMEBALANCING_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEMEBALANCING_H_

#include <memory>

#include "abstractcontrolscheme.h"

namespace Robot::Kinematic {

    class ControlSchemeBalancing : public AbstractControlScheme, public std::enable_shared_from_this<ControlSchemeBalancing> {
        public: 
            explicit ControlSchemeBalancing(std::shared_ptr<Kinematic> kinematic);
            ControlSchemeBalancing(const ControlSchemeBalancing&) = delete; // No copy constructor
            ControlSchemeBalancing(ControlSchemeBalancing&&) = delete; // No move constructor
            virtual ~ControlSchemeBalancing();

            virtual void init() override;
            virtual void cleanup() override;

        private:
    };

};

#endif
