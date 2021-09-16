#ifndef _PRU_DEBUG_H_
#define _PRU_DEBUG_H_

#include <boost/asio.hpp>

#include "component.h"

class PRUDebug : public Component {
    public:
        PRUDebug(boost::asio::io_context &io);

        void init() override;
        void cleanup() override;

    private:
        boost::asio::steady_timer m_timer;

        void timer();
};


#endif
