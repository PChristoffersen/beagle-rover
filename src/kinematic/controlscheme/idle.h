#ifndef _ROBOT_KINEMATIC_CONTROLSCHEMEIDLE_H_
#define _ROBOT_KINEMATIC_CONTROLSCHEMEIDLE_H_

#include <memory>
#include <boost/asio.hpp>

#include "abstractcontrolscheme.h"

namespace Robot::Kinematic {

    class ControlSchemeIdle : public AbstractControlScheme, public std::enable_shared_from_this<ControlSchemeIdle> {
        public: 
            explicit ControlSchemeIdle(std::shared_ptr<Kinematic> kinematic);
            ControlSchemeIdle(const ControlSchemeIdle&) = delete; // No copy constructor
            ControlSchemeIdle(ControlSchemeIdle&&) = delete; // No move constructor
            virtual ~ControlSchemeIdle();

            virtual void init() override;
            virtual void cleanup() override;

        private:
            boost::asio::steady_timer m_timer;

            void timer_setup();
            void timer();
    };

}

#endif
