#ifndef _ROBOT_TELEMETRY_MOTORSOURCE_H_
#define _ROBOT_TELEMETRY_MOTORSOURCE_H_

#include <robotconfig.h>

#include <memory>
#include <boost/asio.hpp>

#include <common/withnotify.h>
#include "abstracttelemetrysource.h"
#include <motor/types.h>
#include "../types.h"

namespace Robot::Telemetry {

    class Motors : public AbstractSource<Motors> {
        public:
            explicit Motors(const std::shared_ptr<Motor::Control> &motor_control);
            Motors(const Motors&) = delete; // No copy constructor
            Motors(Motors&&) = delete; // No move constructor
            virtual ~Motors();

            void init(const std::shared_ptr<Telemetry> &telemetry);
            void cleanup();

        private:
            bool m_initialized;

            std::weak_ptr<Motor::Control> m_motor_control;

            EventMotors m_event;

            boost::signals2::connection m_connection;
            void onMotorsUpdated(const Motor::MotorList &motors);
    };

}

#endif
