#ifndef _TELEMETRY_H_
#define _TELEMETRY_H_

#include <stdint.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

#include "telemetrytypes.h"

class Telemetry : public boost::enable_shared_from_this<Telemetry> {
    public:
        [[nodiscard]] static boost::shared_ptr<Telemetry> create(boost::shared_ptr<class RobotContext> context);

        void init();
        void cleanup();

        TelemetrySignal_t sig_event;

    protected:
        friend class Battery;

        void process(const TelemetryEvent &event);
        void processBattery(const TelemetryEventBattery &event);
        void send(uint16_t appId, uint32_t data);

    private:
        boost::shared_ptr<class RobotContext> m_context;

        boost::shared_ptr<class Battery> m_battery;
        boost::signals2::connection m_battery_connection;

        Telemetry(boost::shared_ptr<class RobotContext> context);
};

#endif
