#ifndef _PRU_DEBUG_H_
#define _PRU_DEBUG_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

class PRUDebug {
    public:
        PRUDebug(boost::shared_ptr<class RobotContext> context);

        void init();
        void cleanup();

    private:
        boost::asio::steady_timer m_timer;

        void timer();
};


#endif
