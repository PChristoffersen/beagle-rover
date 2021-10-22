

#include "abstractcontrolscheme.h"

class ControlBalancing : public AbstractControlScheme {
    public: 
        [[nodiscard]] static boost::shared_ptr<ControlBalancing> create(boost::shared_ptr<class Kinematic> kinematic);

        ~ControlBalancing();

        virtual void init() override;
        virtual void cleanup() override;

    private:
        ControlBalancing(boost::shared_ptr<class Kinematic> kinematic);
};
