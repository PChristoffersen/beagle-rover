#ifndef _TELEMETRY_H_
#define _TELEMETRY_H_

#include <memory>
#include <stdint.h>
#include <boost/asio.hpp>

#include "telemetrytypes.h"

class Telemetry : public std::enable_shared_from_this<Telemetry> {
    public:
        Telemetry(std::shared_ptr<class RobotContext> context);
        virtual ~Telemetry();

        void init();
        void cleanup();

        TelemetrySignal sig_event;

    protected:
        void process(const TelemetryEvent &event);
        void processBattery(const TelemetryEventBattery &event);
        void send(uint16_t appId, uint32_t data);

    private:
        bool m_initialized;
        std::unique_ptr<class ADCBattery> m_battery;
        boost::signals2::connection m_battery_connection;

};

#endif
