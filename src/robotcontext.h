#ifndef _ROBOTCONTEXT_H_
#define _ROBOTCONTEXT_H_

#include <mutex>
#include <memory>
#include <thread>
#include <boost/asio.hpp>

#include "common/withmutex.h"

namespace Robot {

    class Context : public std::enable_shared_from_this<Context>, public WithMutex<std::recursive_mutex> {
        public:
            Context();
            Context(const Context&) = delete; // No copy constructor
            Context(Context&&) = delete; // No move constructor

            virtual ~Context();

            void init();
            void cleanup();

            void start();
            void stop();

            void motorPower(bool enable);
            void servoPower(bool enable);
            void rcPower(bool enable);

            boost::asio::io_context &io() { return m_io; }

        private:
            bool m_initialized;
            bool m_started;
            boost::asio::io_context m_io;
            std::shared_ptr<std::thread> m_thread;
            
            uint m_motor_power_rail_cnt;
            uint m_servo_power_rail_cnt;

            void initLogging();

            void initBeagleBone();
            void cleanupBeagleBone();
            void initPC();
            void cleanupPC();
    };

};

#endif
