#ifndef _RCMPU_H_
#define _RCMPU_H_

#include <memory>
#include <boost/asio.hpp>

#include "abstracttelemetrysource.h"
#include "telemetrytypes.h"

class RCMPU : public AbstractTelemetrySource<RCMPU> {
    public:
        RCMPU(std::shared_ptr<class RobotContext> context);
        virtual ~RCMPU();

        void init();
        void cleanup();

    protected:
        friend class Telemetry;

        TelemetrySignal sig_event;

    private:
        bool m_initialized;
        boost::asio::steady_timer m_timer;

    	rc_mpu_data_t m_data;

        void timer_setup();
        void timer(boost::system::error_code error);
};

#endif
