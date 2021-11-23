#ifndef _RCMPU_H_
#define _RCMPU_H_

#include <memory>
#include <boost/asio.hpp>
#include <robotcontrol.h>

#include "abstracttelemetrysource.h"
#include "../telemetrytypes.h"

namespace Robot::Telemetry {

    class RCMPU : public AbstractSource<RCMPU> {
        public:
            explicit RCMPU(const std::shared_ptr<Robot::Context> &context);
            RCMPU(const RCMPU&) = delete; // No copy constructor
            RCMPU(RCMPU&&) = delete; // No move constructor
            virtual ~RCMPU();

            void init(const std::shared_ptr<Telemetry> &telemetry);
            void cleanup();

        protected:
            friend class Telemetry;

            Signal sig_event;

        private:
            bool m_initialized;
            boost::asio::steady_timer m_timer;

            rc_mpu_data_t m_data;

            void timer_setup();
            void timer(boost::system::error_code error);
    };

        
};

#endif
