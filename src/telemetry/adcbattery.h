#ifndef _BATTERY_H_
#define _BATTERY_H_

#include <memory>
#include <boost/asio.hpp>

#include "abstracttelemetrysource.h"
#include "telemetrytypes.h"

class ADCBattery : public AbstractTelemetrySource<ADCBattery> {
    public:
        explicit ADCBattery(std::shared_ptr<class RobotContext> context);
        ADCBattery(const ADCBattery&) = delete; // No copy constructor
        ADCBattery(ADCBattery&&) = delete; // No move constructor
        virtual ~ADCBattery();

        void init();
        void cleanup();

    private:
        bool m_initialized;
        boost::asio::steady_timer m_timer;

        void timer_setup();
        void timer(boost::system::error_code error);
};

#endif


