#ifndef _ROBOT_TELEMETRY_SOURCE_H_
#define _ROBOT_TELEMETRY_SOURCE_H_

#include <memory>

#include "types.h"
#include "telemetry.h"

namespace Robot::Telemetry {

    class Source {
        public:
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

            void send(const Event &event)
            {
                if (auto telemetry = m_telemetry.lock()) {
                    send(telemetry, event);
                }
            }
            static void send(const std::shared_ptr<Telemetry> &telemetry, const Event &event)
            {
                telemetry->process(event);
            }
            static void send(const std::shared_ptr<Telemetry> &telemetry, const MPUData &data)
            {
                telemetry->process(data);
            }

    };

};

#endif