#ifndef _BATTERY_H_
#define _BATTERY_H_

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "telemetrytypes.h"

class Battery {
    public:
        Battery(boost::shared_ptr<class RobotContext> context);
        ~Battery();

        void init();
        void cleanup();

    protected:
        friend class Telemetry;

        TelemetrySignal_t sig_event;

    private:
        boost::asio::steady_timer m_timer;

        void timer();
        void timerFake();
};

#endif


