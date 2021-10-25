#ifndef _CONTROLIDLE_H_
#define _CONTROLIDLE_H_

#include <mutex>
#include <boost/asio.hpp>

#include "abstractcontrolscheme.h"

class ControlIdle : public AbstractControlScheme, public std::enable_shared_from_this<ControlIdle> {
    public: 
        ControlIdle(std::shared_ptr<class Kinematic> kinematic);
        ~ControlIdle();

        virtual void init() override;
        virtual void cleanup() override;

    private:
        bool m_initialized;
        bool m_timer_pending;
        std::mutex m_mutex;
        boost::asio::steady_timer m_timer;

        void timer(boost::system::error_code error);
};

#endif
