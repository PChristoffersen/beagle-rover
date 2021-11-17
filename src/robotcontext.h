#ifndef _ROBOTCONTEXT_H_
#define _ROBOTCONTEXT_H_

#include <mutex>
#include <memory>
#include <thread>
#include <boost/asio.hpp>

#include "common/withmutex.h"


class RobotContext : public std::enable_shared_from_this<RobotContext>, public WithMutex<std::recursive_mutex> {
    public:
        RobotContext();
        RobotContext(const RobotContext&) = delete; // No copy constructor
        RobotContext(RobotContext&&) = delete; // No move constructor

        virtual ~RobotContext();

        void init();
        void cleanup();

        void start();
        void stop();


        boost::asio::io_context &io() { return m_io; }

    private:
        bool m_initialized;
        bool m_started;
        boost::asio::io_context m_io;
        std::shared_ptr<std::thread> m_thread;
        
        bool m_power_enabled;

        void initLogging();

        void initBeagleBone();
        void cleanupBeagleBone();
        void initPC();
        void cleanupPC();
};


#endif
