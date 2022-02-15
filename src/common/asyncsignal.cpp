#include "asyncsignal.h"

#include <sys/eventfd.h>
#include <boost/throw_exception.hpp>
#include <boost/log/trivial.hpp>

using namespace std::literals;

namespace Robot {

ASyncSignal::ASyncSignal(boost::asio::io_context &io) :
    ASyncSignal { io, 0ms }
{
    
}

ASyncSignal::ASyncSignal(boost::asio::io_context &io, clock_type::duration min_interval) :
    m_fd { -1 }, 
    m_signal_interval { min_interval },
    m_descriptor { io },
    m_timer { io }
{
    m_fd = eventfd(0, EFD_CLOEXEC|EFD_NONBLOCK);
    if (m_fd==-1) {
        BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Error creating eventfd"));
    }
    m_descriptor.assign(m_fd);
    m_last_read = clock_type::now();
}


ASyncSignal::~ASyncSignal()
{
    cancel();
    if (m_fd>=0) {
        close(m_fd);
        m_fd = -1;
    }
}


void ASyncSignal::async_wait()
{
    m_last_read = clock_type::now();
    startRead();
}


void ASyncSignal::cancel()
{
    m_descriptor.cancel();
    m_timer.cancel();
    m_sig.disconnect_all_slots();
}



boost::signals2::connection ASyncSignal::connect(const typename signal_t::slot_type &subscriber) 
{
    return m_sig.connect(subscriber);
}


void ASyncSignal::operator()(uint64_t cnt)
{
    auto res = ::write(m_fd, &cnt, sizeof(cnt));
    if (res!=sizeof(cnt)) {
        BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Error writing to eventfd"));
    }
}




inline void ASyncSignal::read()
{
    uint64_t cnt { 0ULL };
    auto res = ::read(m_fd, &cnt, sizeof(cnt));
    if (res==-1) {
        if (errno==EAGAIN) {
            return;
        }
        BOOST_THROW_EXCEPTION(std::system_error(errno, std::generic_category(), "Error reading from eventfd"));
    }
    m_last_read = clock_type::now();
    m_sig(cnt);
}


void ASyncSignal::startRead()
{
    auto now = clock_type::now();
    auto diff = now-m_last_read;
    if (diff < m_signal_interval) {
        m_timer.expires_after(m_signal_interval-diff);
        m_timer.async_wait([self_ptr=weak_from_this()] (boost::system::error_code error) {
            if (error!=boost::system::errc::success) {
                return;
            }
            if (auto self = self_ptr.lock()) {
                self->read();
                self->startRead(); 
            }
        });
        return;
    }

    m_descriptor.async_read_some(m_buffers, [self_ptr=this->weak_from_this()](const boost::system::error_code& error, std::size_t bytes_transferred) {
        //BOOST_LOG_TRIVIAL(trace) << "Read ec=" << error << "  bytes_transferred=" << bytes_transferred;
        if (error!=boost::system::errc::success) {
            return;
        }
        if (auto self = self_ptr.lock()) { 
            self->read();
            self->startRead();
        }
    });
}

}
