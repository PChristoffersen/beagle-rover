#ifndef _CONTROLSCHEMESPINNING_H_
#define _CONTROLSCHEMESPINNING_H_

#include "abstractcontrolscheme.h"

class ControlSchemeSpinning : public AbstractControlScheme<ControlSchemeSpinning> {
    public: 
        explicit ControlSchemeSpinning(std::shared_ptr<class Kinematic> kinematic);
        ControlSchemeSpinning(const ControlSchemeSpinning&) = delete; // No copy constructor
        ControlSchemeSpinning(ControlSchemeSpinning&&) = delete; // No move constructor
        virtual ~ControlSchemeSpinning();

        virtual void init() override;
        virtual void cleanup() override;

    private:
};

#endif