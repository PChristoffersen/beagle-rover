#ifndef _CONTROLSCHEMENORMAL_H_
#define _CONTROLSCHEMENORMAL_H_

#include "abstractcontrolscheme.h"

class ControlSchemeNormal : public AbstractControlScheme<ControlSchemeNormal> {
    public: 
        ControlSchemeNormal(std::shared_ptr<class Kinematic> kinematic);
        ~ControlSchemeNormal();

        virtual void init() override;
        virtual void cleanup() override;

    private:
};

#endif
