#ifndef _PRU_DEBUG_H_
#define _PRU_DEBUG_H_

#include <memory>
#include <boost/asio.hpp>

class PRUDebug : public std::enable_shared_from_this<PRUDebug> {
    public:
        PRUDebug(std::shared_ptr<class RobotContext> context);
        virtual ~PRUDebug();

        void init();
        void cleanup();

    private:
        bool m_initialized;
        boost::asio::steady_timer m_timer;

        void timer(boost::system::error_code error);
};


#endif
