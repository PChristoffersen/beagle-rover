#ifndef _ROBOT_TELEMETRY_ODOMETERSOURCE_H_
#define _ROBOT_TELEMETRY_ODOMETERSOURCE_H_

#include <robotconfig.h>

#include <memory>
#include <boost/asio.hpp>

#include <common/withnotify.h>
#include "abstracttelemetrysource.h"
#include <kinematic/types.h>
#include "../types.h"

namespace Robot::Telemetry {

    class Odometer : public AbstractSource<Odometer> {
        public:
            using odometer_type = std::int32_t;

            explicit Odometer(const std::shared_ptr<Kinematic::Kinematic> &kinematic);
            Odometer(const Odometer&) = delete; // No copy constructor
            Odometer(Odometer&&) = delete; // No move constructor
            virtual ~Odometer();

            void init(const std::shared_ptr<Telemetry> &telemetry);
            void cleanup();

        private:
            bool m_initialized;

            std::weak_ptr<Kinematic::Kinematic> m_kinematic;

            EventOdometer m_event;

            boost::signals2::connection m_connection;
            void onOdometerUpdated(odometer_type odometer);
    };

}

#endif
