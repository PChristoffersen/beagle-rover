#ifndef _CONTROLSPINNING_H_
#define _CONTROLSPINNING_H_

#include "abstractcontrolscheme.h"

class ControlSpinning : public AbstractControlScheme {
    public: 
        ControlSpinning(std::shared_ptr<class Kinematic> kinematic);
        ~ControlSpinning();

        virtual void init() override;
        virtual void cleanup() override;

    private:
};

#endif
