#ifndef _ROBOT_TELEMETRY_SOURCE_H_
#define _ROBOT_TELEMETRY_SOURCE_H_

#include <memory>

#include "types.h"
#include "telemetry.h"

namespace Robot::Telemetry {

    class Source {
        public:
            Source() {}
            virtual ~Source() = default;

            virtual void init(const std::shared_ptr<Telemetry> &telemetry) 
            {
                m_telemetry = telemetry;
            }
            virtual void cleanup() {
                m_telemetry.reset();
            }

        protected:
            friend class Telemetry;
            std::weak_ptr<Telemetry> m_telemetry;

            void sendEvent(const Event &event)
            {
                if (auto telemetry = m_telemetry.lock()) {
                    sendEvent(telemetry, event);
                }
            }
            static void sendEvent(const std::shared_ptr<Telemetry> &telemetry, const Event &event)
            {
                telemetry->process(event);
            }

            #if ROBOT_HAVE_IMU
            static void sendData(const std::shared_ptr<Telemetry> &telemetry, const IMUData &data)
            {
                telemetry->process(data);
            }
            #endif
    };

}

#endif