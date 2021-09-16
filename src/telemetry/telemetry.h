#ifndef _TELEMETRY_H_
#define _TELEMETRY_H_

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

#include "../component.h"

class Telemetry : public Component, public boost::enable_shared_from_this<Telemetry> {
    public:
        typedef boost::signals2::signal<void(uint16_t,uint32_t)> data_signal_t;

        Telemetry(boost::shared_ptr<boost::asio::io_context> io);

        void init() override;
        void cleanup() override;

        void send(uint16_t appId, uint32_t data);

        data_signal_t sig_data;
    private:
        boost::shared_ptr<class Battery> m_battery;
};

#endif
