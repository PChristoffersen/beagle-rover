#ifndef _PRU_DEBUG_H_
#define _PRU_DEBUG_H_

#include <memory>
#include <chrono>
#include <mutex>
#include <boost/asio.hpp>

#include "../robotcontext.h"

namespace Robot::PRU {

    class Debug : public std::enable_shared_from_this<Debug> {
        public:
            explicit Debug(std::shared_ptr<Robot::Context> context);
            Debug(const Debug&) = delete; // No copy constructor
            Debug(Debug&&) = delete; // No move constructor
            virtual ~Debug();

            void init();
            void cleanup();

        private:
            bool m_initialized;
            std::mutex m_mutex;
            boost::asio::steady_timer m_timer;

            void timer_setup();
            void timer(boost::system::error_code error);
    };

};

#endif
