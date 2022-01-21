#ifndef _ROBOT_CONTEXT_H_
#define _ROBOT_CONTEXT_H_

#include <mutex>
#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <robotconfig.h>
#include <common/withmutex.h>

namespace Robot {

    class Context : public std::enable_shared_from_this<Context>, public WithMutex<std::recursive_mutex> {
        public:
            using PowerSignal = boost::signals2::signal<void(bool)>;

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
            bool motorPower() const { return m_motor_power_rail_cnt>0; }
            bool servoPower() const { return m_servo_power_rail_cnt>0; }
            bool rcPower() const { return servoPower(); }

            boost::asio::io_context &io() { return m_io; }

            PowerSignal sig_motor_power;
            PowerSignal sig_servo_power;
            PowerSignal sig_rc_power;

        private:
            bool m_initialized;
            bool m_started;
            boost::asio::io_context m_io;
            std::shared_ptr<std::thread> m_thread;
            
            uint m_motor_power_rail_cnt;
            uint m_servo_power_rail_cnt;
            uint m_rc_power_rail_cnt;

            void initLogging();

            void initPlatform();
            void cleanupPlatform();
    };

};

#endif
