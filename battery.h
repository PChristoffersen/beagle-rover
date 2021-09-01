#ifndef _BATTERY_H_
#define _BATTERY_H_

#include <boost/asio.hpp>

#include "component.h"

class Battery : public Component{
    public:
        Battery(boost::asio::io_context &io);
        ~Battery();

        void init() override;
        void cleanup() override;
    private:
        boost::asio::steady_timer m_timer;

        void timer();
};

#endif


