#ifndef _CONTROLSCHEMEIDLE_H_
#define _CONTROLSCHEMEIDLE_H_

#include <mutex>
#include <boost/asio.hpp>

#include "abstractcontrolscheme.h"

namespace Robot::Kinematic {

    class ControlSchemeIdle : public AbstractControlScheme<ControlSchemeIdle> {
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
            void timer(boost::system::error_code error);
    };

};

#endif
