#ifndef _ROBOT_ASYNCQUEUE_H_
#define _ROBOT_ASYNCQUEUE_H_

#include <memory>
#include <cstdint>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include <robotconfig.h>

namespace Robot {

    class ASyncSignal : public std::enable_shared_from_this<ASyncSignal> {
        public:
            using signal_type = boost::signals2::signal<void(std::uint64_t)>;
            using timer_type = boost::asio::high_resolution_timer;
            using clock_type = timer_type::clock_type;

            ASyncSignal(boost::asio::io_context &io);
            ASyncSignal(boost::asio::io_context &io, clock_type::duration min_interval);
            ~ASyncSignal();

            void async_wait();
            void cancel();

            void operator()(std::uint64_t cnt = 1);

            boost::signals2::connection connect(const typename signal_type::slot_type &subscriber);
        private:
            int m_fd;
            clock_type::duration m_signal_interval;
            signal_type m_sig;
            boost::asio::posix::stream_descriptor m_descriptor;
            boost::asio::null_buffers m_buffers;
            timer_type m_timer;
            clock_type::time_point m_last_read;

            inline void read();
            void startRead();
    };
}

#endif
