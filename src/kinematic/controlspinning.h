

#include "abstractcontrolscheme.h"

class ControlSpinning : public AbstractControlScheme {
    public: 
        [[nodiscard]] static boost::shared_ptr<ControlSpinning> create(boost::shared_ptr<class Kinematic> kinematic);

        ~ControlSpinning();

        virtual void init() override;
        virtual void cleanup() override;

    private:
        ControlSpinning(boost::shared_ptr<class Kinematic> kinematic);
};
