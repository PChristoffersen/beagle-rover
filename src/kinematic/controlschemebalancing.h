#ifndef _CONTROLSCHEMEBALANCING_H_
#define _CONTROLSCHEMEBALANCING_H_

#include "abstractcontrolscheme.h"

class ControlSchemeBalancing : public AbstractControlScheme<ControlSchemeBalancing> {
    public: 
        explicit ControlSchemeBalancing(std::shared_ptr<class Kinematic> kinematic);
        ControlSchemeBalancing(const ControlSchemeBalancing&) = delete; // No copy constructor
        ControlSchemeBalancing(ControlSchemeBalancing&&) = delete; // No move constructor
        virtual ~ControlSchemeBalancing();

        virtual void init() override;
        virtual void cleanup() override;

    private:
};

#endif