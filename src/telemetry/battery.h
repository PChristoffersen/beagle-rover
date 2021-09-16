#ifndef _BATTERY_H_
#define _BATTERY_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "../component.h"

class Battery : public Component {
    public:
        Battery(boost::shared_ptr<boost::asio::io_context> io, boost::shared_ptr<class Telemetry> telemetry);
        ~Battery();

        void init() override;
        void cleanup() override;

    private:
        boost::weak_ptr<class Telemetry> m_telemetry;
        boost::asio::steady_timer m_timer;

        void timer();
};

#endif


