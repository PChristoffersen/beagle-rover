#ifndef _ROBOTCONTEXT_H_
#define _ROBOTCONTEXT_H_

#include <mutex>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

class RobotContext : public boost::enable_shared_from_this<RobotContext> {
    public:
        [[nodiscard]] static boost::shared_ptr<RobotContext> create();

        virtual ~RobotContext();

        void init();
        void cleanup();

        void start();
        void stop();

        boost::shared_ptr<boost::asio::io_context> io() const {
            return m_io;
        }

    private:
        bool m_initialized;
        bool m_started;
        std::mutex m_mutex;
        boost::shared_ptr<boost::asio::io_context> m_io;
        boost::shared_ptr<boost::thread> m_thread;
        
        bool m_power_enabled;

        RobotContext();

        void initBeagleBone();
        void cleanupBeagleBone();
        void initPC();
        void cleanupPC();
};


#endif
