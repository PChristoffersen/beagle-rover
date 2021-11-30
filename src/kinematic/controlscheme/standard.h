#ifndef _CONTROLSCHEMESTANDARD_H_
#define _CONTROLSCHEMESTANDARD_H_

#include "abstractcontrolscheme.h"

namespace Robot::Kinematic {

    class ControlSchemeStandard : public AbstractControlScheme<ControlSchemeStandard> {
        public: 
            explicit ControlSchemeStandard(std::shared_ptr<Kinematic> kinematic);
            ControlSchemeStandard(const ControlSchemeStandard&) = delete; // No copy constructor
            ControlSchemeStandard(ControlSchemeStandard&&) = delete; // No move constructor
            virtual ~ControlSchemeStandard();

            virtual void init() override;
            virtual void cleanup() override;

            virtual void steer(double steering, double throttle, double aux_x, double aux_y);
        private:
            void steerAll(double steering, double throttle);

    };

};

#endif
