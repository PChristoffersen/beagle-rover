#ifndef _ROBOT_KINEMATIC_CONTROLSCHEMEPASSTHROUGH_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEMEPASSTHROUGH_H_

#include <memory>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <robotconfig.h>
#include <rc/types.h>
#include "abstractcontrolscheme.h"

namespace Robot::Kinematic {

    class ControlSchemePassthrough : public AbstractControlScheme, public std::enable_shared_from_this<ControlSchemePassthrough> {
        public: 
            explicit ControlSchemePassthrough(std::shared_ptr<Kinematic> kinematic);
            ControlSchemePassthrough(const ControlSchemePassthrough&) = delete; // No copy constructor
            ControlSchemePassthrough(ControlSchemePassthrough&&) = delete; // No move constructor
            virtual ~ControlSchemePassthrough();

            virtual void init() override;
            virtual void cleanup() override;

    };

}

#endif
