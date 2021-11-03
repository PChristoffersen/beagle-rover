#ifndef _CONTROLSCHEMESPINNING_H_
#define _CONTROLSCHEMESPINNING_H_

#include "abstractcontrolscheme.h"

class ControlSchemeSpinning : public AbstractControlScheme<ControlSchemeSpinning> {
    public: 
        ControlSchemeSpinning(std::shared_ptr<class Kinematic> kinematic);
        ~ControlSchemeSpinning();

        virtual void init() override;
        virtual void cleanup() override;

    private:
};

#endif
