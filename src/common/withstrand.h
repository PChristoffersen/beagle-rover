#ifndef _ROBOT_WITHSTRAND_H_
#define _ROBOT_WITHSTRAND_H_

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <robotcontext.h>

namespace Robot {

    class WithStrand {
        public:
            using strand_type = boost::asio::io_context::strand;

            WithStrand(boost::asio::io_context &io) : m_strand { io } {}
            WithStrand(const strand_type &strand) : m_strand { strand } {}

            strand_type &strand() const { return m_strand; }
        protected:
            mutable strand_type m_strand;

            template<typename Handler>
            void dispatch(Handler handler) 
            {
                boost::asio::dispatch(m_strand, handler);
            }

            template<typename Handler>
            void defer(Handler handler) 
            {
                boost::asio::defer(m_strand, handler);
            }

            template<typename Handler>
            void post(Handler handler) 
            {
                boost::asio::post(m_strand, handler);
            }
    };

}

#endif
