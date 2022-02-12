#ifndef _ROBOT_HARDWARE_BEAGLEBONE_PRUDEBUG_H_
#define _ROBOT_HARDWARE_BEAGLEBONE_PRUDEBUG_H_

#include <robotconfig.h>

#if ROBOT_PLATFORM == ROBOT_PLATFORM_BEAGLEBONE

#include <memory>
#include <chrono>
#include <mutex>
#include <boost/asio.hpp>

#include <robottypes.h>
#include <common/withmutex.h>

namespace Robot::Hardware::Beaglebone {

    class PRUDebug : public std::enable_shared_from_this<PRUDebug>, public WithMutex<std::mutex> {
        public:
            using timer_type = boost::asio::steady_timer;

            explicit PRUDebug(const std::shared_ptr<::Robot::Context> &context);
            PRUDebug(const PRUDebug&) = delete; // No copy constructor
            PRUDebug(PRUDebug&&) = delete; // No move constructor
            virtual ~PRUDebug();

            void init();
            void cleanup();

        private:
            bool m_initialized;
            timer_type m_timer;

            void timer_setup();
            void timer(boost::system::error_code error);
    };

}

#endif
#endif
