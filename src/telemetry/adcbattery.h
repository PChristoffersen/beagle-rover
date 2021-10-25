#ifndef _BATTERY_H_
#define _BATTERY_H_

#include <memory>
#include <boost/asio.hpp>

#include "telemetrytypes.h"

class ADCBattery : public std::enable_shared_from_this<ADCBattery> {
    public:
        [[nodiscard]] static std::shared_ptr<ADCBattery> create(std::shared_ptr<class RobotContext> context);

        virtual ~ADCBattery();

        void init();
        void cleanup();

    protected:
        friend class Telemetry;

        TelemetrySignal sig_event;

    private:
        bool m_initialized;
        boost::asio::steady_timer m_timer;

        ADCBattery(std::shared_ptr<class RobotContext> context);

        void timer(boost::system::error_code error);
};

#endif


