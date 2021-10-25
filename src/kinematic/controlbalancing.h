#ifndef _CONTROLBALANCING_H_
#define _CONTROLBALANCING_H_

#include "abstractcontrolscheme.h"

class ControlBalancing : public AbstractControlScheme {
    public: 
        ControlBalancing(std::shared_ptr<class Kinematic> kinematic);
        ~ControlBalancing();

        virtual void init() override;
        virtual void cleanup() override;

    private:
};

#endif
