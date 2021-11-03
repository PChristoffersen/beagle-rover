#ifndef _CONTROLSCHEMEIDLE_H_
#define _CONTROLSCHEMEIDLE_H_

#include <mutex>
#include <boost/asio.hpp>

#include "abstractcontrolscheme.h"

class ControlSchemeIdle : public AbstractControlScheme<ControlSchemeIdle> {
    public: 
        ControlSchemeIdle(std::shared_ptr<class Kinematic> kinematic);
        ~ControlSchemeIdle();

        virtual void init() override;
        virtual void cleanup() override;

    private:
        boost::asio::steady_timer m_timer;

        void timer(boost::system::error_code error);
};

#endif
