#ifndef _CONTROLSCHEMEBALANCING_H_
#define _CONTROLSCHEMEBALANCING_H_

#include "abstractcontrolscheme.h"

class ControlSchemeBalancing : public AbstractControlScheme<ControlSchemeBalancing> {
    public: 
        ControlSchemeBalancing(std::shared_ptr<class Kinematic> kinematic);
        ~ControlSchemeBalancing();

        virtual void init() override;
        virtual void cleanup() override;

    private:
};

#endif
