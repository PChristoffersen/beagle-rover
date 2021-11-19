#ifndef _CONTROLSCHEMENORMAL_H_
#define _CONTROLSCHEMENORMAL_H_

#include "abstractcontrolscheme.h"

namespace Robot::Kinematic {

    class ControlSchemeNormal : public AbstractControlScheme<ControlSchemeNormal> {
        public: 
            explicit ControlSchemeNormal(std::shared_ptr<Kinematic> kinematic);
            ControlSchemeNormal(const ControlSchemeNormal&) = delete; // No copy constructor
            ControlSchemeNormal(ControlSchemeNormal&&) = delete; // No move constructor
            virtual ~ControlSchemeNormal();

            virtual void init() override;
            virtual void cleanup() override;

        private:
    };

};

#endif
