#ifndef _ROBOT_TELEMETRY_ROBOTCONTROLBATTERY_H_
#define _ROBOT_TELEMETRY_ROBOTCONTROLBATTERY_H_

#include <robotconfig.h>
#if ROBOT_HAVE_ROBOTCONTROL_BATTERY
#include <memory>
#include <boost/asio.hpp>
#include <robotcontrol.h>

#include <common/withstrand.h>
#include <common/withnotify.h>
#include "abstracttelemetrysource.h"
#include "../types.h"

namespace Robot::Telemetry {

    class RobotControlBattery : public AbstractSource<RobotControlBattery>, public WithStrand {
        public:
            explicit RobotControlBattery(const std::shared_ptr<::Robot::Context> &context);
            RobotControlBattery(const RobotControlBattery&) = delete; // No copy constructor
            RobotControlBattery(RobotControlBattery&&) = delete; // No move constructor
            virtual ~RobotControlBattery();

            void init(const std::shared_ptr<Telemetry> &telemetry);
            void cleanup();

        private:
            bool m_initialized;
            boost::asio::steady_timer m_timer;
            rc_filter_t m_pack_filter;
	        rc_filter_t m_jack_filter;
            double m_pack_voltage;
            double m_jack_voltage;

            inline void timer();
            void timer_setup();
    };

}

#endif
#endif
