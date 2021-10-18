#ifndef _ROBOTCONTEXT_H_
#define _ROBOTCONTEXT_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

class RobotContext : public boost::enable_shared_from_this<RobotContext> {
    public:
        RobotContext();
        ~RobotContext();

        void start();
        void stop();

        boost::shared_ptr<boost::asio::io_context> io() const {
            return m_io;
        }

    private:
        bool m_started;
        boost::shared_ptr<boost::asio::io_context> m_io;
        boost::shared_ptr<boost::thread> m_thread;
};


#endif
