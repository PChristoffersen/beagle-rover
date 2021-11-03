#ifndef _PRU_DEBUG_H_
#define _PRU_DEBUG_H_

#include <memory>
#include <chrono>
#include <mutex>
#include <boost/asio.hpp>

class PRUDebug : public std::enable_shared_from_this<PRUDebug> {
    public:
        PRUDebug(std::shared_ptr<class RobotContext> context);
        virtual ~PRUDebug();

        void init();
        void cleanup();

    private:
        static constexpr auto TIMER_INTERVAL { std::chrono::milliseconds(100) };

        bool m_initialized;
        std::mutex m_mutex;
        boost::asio::steady_timer m_timer;

        void timer_setup();
        void timer(boost::system::error_code error);
};


#endif
