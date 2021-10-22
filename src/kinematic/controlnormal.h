

#include "abstractcontrolscheme.h"

class ControlNormal : public AbstractControlScheme {
    public: 
        ~ControlNormal();

        [[nodiscard]] static boost::shared_ptr<ControlNormal> create(boost::shared_ptr<class Kinematic> kinematic);

        virtual void init() override;
        virtual void cleanup() override;

    private:
        ControlNormal(boost::shared_ptr<class Kinematic> kinematic);
};
