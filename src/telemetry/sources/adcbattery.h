#ifndef _ROBOT_TELEMETRY_BATTERY_H_
#define _ROBOT_TELEMETRY_BATTERY_H_

#include <memory>
#include <boost/asio.hpp>

#include "abstracttelemetrysource.h"
#include "../types.h"

namespace Robot::Telemetry {

    class ADCBattery : public AbstractSource<ADCBattery> {
        public:
            explicit ADCBattery(const std::shared_ptr<Robot::Context> &context);
            ADCBattery(const ADCBattery&) = delete; // No copy constructor
            ADCBattery(ADCBattery&&) = delete; // No move constructor
            virtual ~ADCBattery();

            void init(const std::shared_ptr<Telemetry> &telemetry);
            void cleanup();

        private:
            bool m_initialized;
            boost::asio::steady_timer m_timer;

            void timer_setup();
            void timer(boost::system::error_code error);
    };

};

#endif


