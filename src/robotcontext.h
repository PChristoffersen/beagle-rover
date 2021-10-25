#ifndef _ROBOTCONTEXT_H_
#define _ROBOTCONTEXT_H_

#include <mutex>
#include <memory>
#include <thread>
#include <boost/asio.hpp>

class RobotContext : public std::enable_shared_from_this<RobotContext> {
    public:
        RobotContext();
        virtual ~RobotContext();

        void init();
        void cleanup();

        void start();
        void stop();



        boost::asio::io_context &io() { return m_io; }

    private:
        bool m_initialized;
        bool m_started;
        std::mutex m_mutex;
        boost::asio::io_context m_io;
        std::shared_ptr<std::thread> m_thread;
        
        bool m_power_enabled;

        void initBeagleBone();
        void cleanupBeagleBone();
        void initPC();
        void cleanupPC();
};


#endif
