#ifndef _PRU_DEBUG_H_
#define _PRU_DEBUG_H_

#include <memory>
#include <chrono>
#include <mutex>
#include <boost/asio.hpp>

class PRUDebug : public std::enable_shared_from_this<PRUDebug> {
    public:
        explicit PRUDebug(std::shared_ptr<class RobotContext> context);
        PRUDebug(const PRUDebug&) = delete; // No copy constructor
        PRUDebug(PRUDebug&&) = delete; // No move constructor
        virtual ~PRUDebug();

        void init();
        void cleanup();

    private:
        bool m_initialized;
        std::mutex m_mutex;
        boost::asio::steady_timer m_timer;

        void timer_setup();
        void timer(boost::system::error_code error);
};


#endif
