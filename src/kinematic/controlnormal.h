#ifndef _CONTROLNORMAL_H_
#define _CONTROLNORMAL_H_

#include "abstractcontrolscheme.h"

class ControlNormal : public AbstractControlScheme {
    public: 
        ControlNormal(std::shared_ptr<class Kinematic> kinematic);
        ~ControlNormal();

        virtual void init() override;
        virtual void cleanup() override;

    private:
};

#endif
