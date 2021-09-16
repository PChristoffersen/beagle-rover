#ifndef _PRU_DEBUG_H_
#define _PRU_DEBUG_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "component.h"

class PRUDebug : public Component {
    public:
        PRUDebug(boost::shared_ptr<boost::asio::io_context> io);

        void init() override;
        void cleanup() override;

    private:
        boost::asio::steady_timer m_timer;

        void timer();
};


#endif
